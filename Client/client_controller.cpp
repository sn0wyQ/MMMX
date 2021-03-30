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

  // TODO(Everyone): Send nickname to server after connection

  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kGameFinished;
  view_->Update();
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_->SetLocalPlayerId(event.GetArg<GameObjectId>(0));
  qInfo().noquote() << "[CLIENT] Set player_id to"
                    << event.GetArg<GameObjectId>(0);
}

void ClientController::StartGameEvent(const Event& event) {
  game_state_ = GameState::kGameInProgress;
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::OnTick(int delta_time) {
  switch (game_state_) {
    case GameState::kGameFinished:
      this->OnTickGameFinished(delta_time);
      break;

    case GameState::kGameInProgress:
      this->OnTickGameInProgress(delta_time);
      break;

    case GameState::kGameNotStarted:
      this->OnTickGameNotStarted(delta_time);
      break;
  }

  view_->Update();
}

void ClientController::OnTickGameNotStarted(int delta_time) {
  // Temporary
  this->OnTickGameInProgress(delta_time);
}

void ClientController::OnTickGameInProgress(int delta_time) {
  this->CollidePlayerWithGameObjects(delta_time);
  this->UpdateLocalPlayer(delta_time);
  this->TickPlayers(delta_time);
}

void ClientController::CollidePlayerWithGameObjects(
    int delta_time) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto local_player = model_->GetLocalPlayer();
  QVector2D key_force = GetKeyForce();
  if (key_force.isNull()) {
    return;
  }
  local_player->SetVelocity(key_force);
  ObjectCollision::DoFirstPhase(
      local_player, model_->GetAllMovableObjects(),
      delta_time);
  ObjectCollision::DoFirstPhase(
      local_player, model_->GetAllRectangularStaticObjects(),
      delta_time);
  ObjectCollision::DoFirstPhase(
      local_player, model_->GetAllRoundStaticObjects(),
      delta_time);
  if (!local_player->GetVelocity().isNull()) {
    return;
  }
  std::vector<QVector2D> tangents;
  ObjectCollision::FindTangents(
      local_player, model_->GetAllMovableObjects(), &tangents);
  ObjectCollision::FindTangents(
      local_player, model_->GetAllRectangularStaticObjects(), &tangents);
  ObjectCollision::FindTangents(
      local_player, model_->GetAllRoundStaticObjects(), &tangents);
  ObjectCollision::DoSecondPhase(
      local_player, model_->GetAllMovableObjects(), key_force,
      tangents);
  ObjectCollision::DoSecondPhase(
      local_player, model_->GetAllRectangularStaticObjects(), key_force,
      tangents);
  ObjectCollision::DoSecondPhase(
      local_player, model_->GetAllRoundStaticObjects(), key_force,
      tangents);
}

void ClientController::TickPlayers(int delta_time) {
  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->OnTick(delta_time);
  }
}

void ClientController::UpdateLocalPlayer(int delta_time) {
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
                               local_player->GetRotation()));
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_->DeleteGameObject(event.GetArg<GameObjectId>(0));
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

QVector2D ClientController::GetKeyForce() const {
  bool is_up_pressed = is_direction_by_keys_.at(Direction::kUp);
  bool is_right_pressed = is_direction_by_keys_.at(Direction::kRight);
  bool is_down_pressed = is_direction_by_keys_.at(Direction::kDown);
  bool is_left_pressed = is_direction_by_keys_.at(Direction::kLeft);

  QVector2D key_force;
  if ((is_up_pressed ^ is_down_pressed) == 1) {
    if (is_up_pressed) {
      key_force.setY(-1.f);
    } else {
      key_force.setY(1.f);
    }
  }
  if ((is_right_pressed ^ is_left_pressed) == 1) {
    if (is_right_pressed) {
      key_force.setX(1.f);
    } else {
      key_force.setX(-1.f);
    }
  }

  key_force.normalize();
  return key_force;
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

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {
  if (model_->IsLocalPlayerSet()) {
    auto local_player = model_->GetLocalPlayer();
    float rotation = Math::DirectionAngle(local_player->GetPosition(),
                                            converter_->PointFromScreenToGame(
                                                mouse_event->pos()));
    local_player->SetRotation(rotation);
  }
}

// ------------------- GAME EVENTS -------------------

void ClientController::SendControlsEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ClientController::UpdateGameObjectDataEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto game_object_type
      = static_cast<GameObjectType>(event.GetArg<int>(1));
  auto params = event.GetArgsSubVector(2);

  if (model_->IsGameObjectIdTaken(game_object_id)) {
    if (game_object_type == GameObjectType::kPlayer
      && game_object_id == model_->GetLocalPlayer()->GetId()) {
      return;
    }
    model_->GetGameObjectByGameObjectId(game_object_id)->SetParams(params);
  } else {
    model_->AddGameObject(game_object_id, game_object_type, params);
  }
  view_->Update();
}
