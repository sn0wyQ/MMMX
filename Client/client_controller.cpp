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
          &ClientController::UpdateVarsAndPing);
  timer_for_server_var_.start(Constants::kTimeToUpdateVarsAndPing);
  connect(&web_socket_, &QWebSocket::pong, this,
          &ClientController::SetPing);

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
  game_state_ = GameState::kGameFinished;
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
  switch (game_state_) {
    case GameState::kGameFinished:
      this->OnTickGameFinished(time_from_previous_tick);
      break;

    case GameState::kGameInProgress:
      this->OnTickGameInProgress(time_from_previous_tick);
      break;

    case GameState::kGameNotStarted:
      this->OnTickGameNotStarted(time_from_previous_tick);
      break;
  }
}

void ClientController::OnTickGameInProgress(int time_from_previous_tick) {
  this->TickPlayers(time_from_previous_tick);
  this->UpdateLocalPlayer(time_from_previous_tick);
}

void ClientController::TickPlayers(int time_from_previous_tick) {
  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->OnTick(time_from_previous_tick);
  }
}

void ClientController::UpdateLocalPlayer(int time_from_previous_tick) {
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
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_->DeletePlayer(event.GetArg<GameObjectId>(0));
  game_state_ = GameState::kGameNotStarted;
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

int ClientController::GetRoomVar() const {
  return room_var_;
}

int ClientController::GetClientVar() const {
  return client_var_;
}

void ClientController::SetPing(int elapsed_time) {
  ping_ = elapsed_time;
}

void ClientController::UpdateVarsAndPing() {
  this->AddEventToSend(Event(EventType::kSendGetVarsEvent));
  web_socket_.ping();
}

void ClientController::UpdateVarsEvent(const Event& event) {
  server_var_ = event.GetArg<int>(0);
  room_var_ = event.GetArg<int>(1);
  client_var_ = this->GetVar();
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
  player_ptr->SetIsInFov(true);

  view_->Update();
}

void ClientController::UpdatePlayersFovEvent(const Event& event) {
  model_->GetLocalPlayer()->SetFovRadius(event.GetArg<float>(1));
  qDebug() << "[CLIENT] Set player FOV to" <<
                                           model_->GetLocalPlayer()->GetFovRadius();
}

void ClientController::PlayerLeftFovEvent(const Event& event) {
  model_->GetPlayerByPlayerId(
      event.GetArg<GameObjectId>(0))->SetIsInFov(false);
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

  this->ApplyDirection();
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = false;
  }

  this->ApplyDirection();
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

  uint32_t direction_mask = is_direction_applied_[Direction::kUp] * 0b1000
      + is_direction_applied_[Direction::kRight] * 0b0100
      + is_direction_applied_[Direction::kDown] * 0b0010
      + is_direction_applied_[Direction::kLeft] * 0b0001;

  model_->GetLocalPlayer()->UpdateVelocity(direction_mask);
  view_->Update();
}

void ClientController::ResetDirection() {
  is_direction_applied_[Direction::kUp]
      = is_direction_applied_[Direction::kRight]
      = is_direction_applied_[Direction::kDown]
      = is_direction_applied_[Direction::kLeft] = false;
}
