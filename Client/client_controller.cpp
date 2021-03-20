#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url),
  model_(std::make_shared<GameDataModel>()) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  connect(&web_socket_, &QWebSocket::connected, this,
          &ClientController::OnConnected);
  connect(&web_socket_, &QWebSocket::disconnected, this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  this->StartTicking();
}

std::shared_ptr<GameDataModel> ClientController::GetModel() {
  return model_;
}

bool ClientController::IsGameInProgress() const {
  return game_state_ == GameState::kGameInProgress;
}

void ClientController::OnConnected() {
  connect(&web_socket_,
          &QWebSocket::binaryMessageReceived,
          this,
          &ClientController::OnByteArrayReceived);

  connect(&timer_for_server_var_, &QTimer::timeout, this,
          &ClientController::UpdateServerVar);
  timer_for_server_var_.start(Constants::kTimeToUpdateServerVar);
  timer_elapsed_server_var_.start();
  connect(&web_socket_, &QWebSocket::pong, this,
          &ClientController::UpdatePing);

  view_->Update();

  // TODO(Everyone): Send nickname to server after connection

  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::AddNewPlayerEvent(const Event& event) {
  model_->AddPlayer(event.GetArg<GameObjectId>(1));
  view_->Update();
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kFinished;
  view_->Update();
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_->SetLocalPlayerId(event.GetArg<GameObjectId>(1));
  qInfo().noquote() << "[CLIENT] Set player_id to"
                    << event.GetArg<GameObjectId>(1);
}

void ClientController::CreateAllPlayersDataEvent(const Event& event) {
  for (int i = 2; i < event.GetArg<int>(1) * 4 + 2; i += 4) {
    model_->AddPlayer(event.GetArg<GameObjectId>(i),
                     event.GetArg<float>(i + 1),
                     event.GetArg<float>(i + 2),
                     event.GetArg<float>(i + 3));
  }
  view_->Update();
}

void ClientController::StartGameEvent(const Event& event) {
  game_state_ = GameState::kGameInProgress;
  view_->Update();
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::OnTick(int time_from_previous_tick) {
  if (model_->IsLocalPlayerSet()) {
    auto local_player = model_->GetLocalPlayer();
    for (const auto& item : model_->GetAllGameObjects()) {
      if (local_player->GetId() == item->GetId()) {
        continue;
      }
      QVector2D offset = QVector2D(item->GetX() - local_player->GetX(),
                               item->GetY() - local_player->GetY())
        - local_player->GetAppliedDeltaPosition(time_from_previous_tick);
      if (IntersectChecker::IsIntersectBodies(local_player->GetRigidBody(),
                                              item->GetRigidBody(), offset)) {
        qInfo() << item->GetId() << " " << local_player->GetId()
                << "intersect";
        local_player->SetVelocity({0.f, 0.f});
      }
    }
  }

  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->OnTick(time_from_previous_tick);
  }

  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();

  converter_->UpdateGameCenter(local_player->GetPosition());

  this->AddEventToHandle(Event(EventType::kSendControls,
                               local_player->GetId(),
                               local_player->GetX(),
                               local_player->GetY(),
                               local_player->GetVelocity(),
                               local_player->GetViewAngle()));

  view_->Update();
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_->DeletePlayer(event.GetArg<GameObjectId>(0));
  game_state_ = GameState::kNotStarted;
  view_->Update();
}

void ClientController::SetView(std::shared_ptr<AbstractClientView> view) {
  view_ = std::move(view);
  converter_ = view_->GetConverter();
}

QString ClientController::GetControllerName() const {
  return "CLIENT";
}

int ClientController::GetPing() const {
  return ping_;
}

int ClientController::GetServerVar() const {
  return server_var_;
}

void ClientController::UpdatePing(int elapsed_time) {
  ping_ = elapsed_time;
}

void ClientController::UpdateServerVar() {
  this->AddEventToSend(Event(EventType::kUpdateServerVar,
                             model_->GetLocalPlayerId()));
  timer_elapsed_server_var_.restart();
  web_socket_.ping();
}

void ClientController::UpdateServerVarEvent(const Event& event) {
  server_var_ = static_cast<int>(timer_elapsed_server_var_.elapsed()) / 2;
  view_->Update();
}

// ------------------- GAME EVENTS -------------------

void ClientController::SendControlsEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ClientController::UpdatePlayerDataEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto player_ptr = model_->GetPlayerByPlayerId(event.GetArg<GameObjectId>(0));

  if (player_ptr->IsLocalPlayer()) {
    return;
  }

  player_ptr->SetX(event.GetArg<float>(1));
  player_ptr->SetY(event.GetArg<float>(2));
  player_ptr->SetVelocity(event.GetArg<QVector2D>(3));
  player_ptr->SetViewAngle(event.GetArg<float>(4));

  view_->Update();
}

void ClientController::GameObjectAppearedEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto game_object_type
      = static_cast<GameObjectType>(event.GetArg<int>(1));
  switch (game_object_type) {
    case GameObjectType::kBox:
      model_->AddBox(game_object_id,
                     std::make_shared<Box>(
                         event.GetArg<int>(2),
                         event.GetArg<int>(3)));
      break;
  }
}

// -------------------- CONTROLS --------------------

void ClientController::FocusOutEvent(QFocusEvent* focus_event) {
  for (const auto& [key, direction] : key_to_direction_) {
    is_direction_by_keys_[direction] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity({0, 0});
  }
}

void ClientController::KeyPressEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = true;
  }

  ApplyDirection();
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = false;
  }

  ApplyDirection();
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {
  if (model_->IsLocalPlayerSet()) {
    auto local_player = model_->GetLocalPlayer();
    float view_angle = Math::DirectionAngle(local_player->GetPosition(),
                                            converter_->PointFromScreenToGame(
                                                mouse_event->pos()));
    local_player->SetViewAngle(view_angle);
    view_->Update();
  }
}

void ClientController::ApplyDirection() {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  ResetDirection();

  bool is_up_pressed = is_direction_by_keys_[Direction::kUp];
  bool is_right_pressed = is_direction_by_keys_[Direction::kRight];
  bool is_down_pressed = is_direction_by_keys_[Direction::kDown];
  bool is_left_pressed = is_direction_by_keys_[Direction::kLeft];

  if ((is_up_pressed ^ is_down_pressed) == 1) {
    is_direction_applied_[is_up_pressed ? Direction::kUp : Direction::kDown]
        = true;
  }
  if ((is_right_pressed ^ is_left_pressed) == 1) {
    is_direction_applied_[is_right_pressed ?
                          Direction::kRight : Direction::kLeft] = true;
  }

  uint32_t direction_mask = is_direction_applied_[Direction::kUp] * 8
      + is_direction_applied_[Direction::kRight] * 4
      + is_direction_applied_[Direction::kDown] * 2
      + is_direction_applied_[Direction::kLeft];

  model_->GetLocalPlayer()->UpdateVelocity(direction_mask);
  view_->Update();
}

void ClientController::ResetDirection() {
  is_direction_applied_[Direction::kUp]
      = is_direction_applied_[Direction::kRight]
      = is_direction_applied_[Direction::kDown]
      = is_direction_applied_[Direction::kLeft] = false;
}
