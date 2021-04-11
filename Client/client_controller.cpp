#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url),
  model_(std::make_shared<ClientGameModel>()) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  connect(&web_socket_, &QWebSocket::connected, this,
          &ClientController::OnConnected);
  connect(&web_socket_, &QWebSocket::disconnected, this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  this->StartTicking();
}

std::shared_ptr<ClientGameModel> ClientController::GetModel() {
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

  this->SendEvent(Event(EventType::kSetTimeDifference,
                  QDateTime::currentMSecsSinceEpoch()));
  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  Event event(message);
  if (event.GetType() == EventType::kSetTimeDifference) {
    auto client_sent_time = event.GetArg<int64_t>(0);
    auto server_received_time = event.GetArg<int64_t>(1);
    int64_t client_received_time = QDateTime::currentMSecsSinceEpoch();
    int64_t latency = (client_received_time - client_sent_time) / 2;
    time_difference_ = server_received_time - client_sent_time - latency;
    qInfo() << time_difference_;
    is_time_difference_set_ = true;
    return;
  }
  this->AddEventToHandle(event);
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kGameFinished;
  view_->Update();
}

void ClientController::SetPlayerIdToClient(const Event& event) {
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
  if (!is_time_difference_set_) {
    return;
  }
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
  for (const auto& [game_object_id, deque] : game_objects_cache_) {
    while (!deque.empty()) {
      auto game_object_data = deque.front();
      auto server_sent_time = game_object_data.server_time;
      auto cur_time_on_server =
          QDateTime::currentMSecsSinceEpoch() + time_difference_;
      auto time_to_interpolate = cur_time_on_server
          - Constants::kInterpolationMSecs;
      // int64_t ccc = 1618150310011;
      // qInfo() << server_sent_time - ccc << cur_time_on_server - ccc
      //         << time_to_interpolate - ccc;
      if (server_sent_time >= time_to_interpolate) {
        break;
      }
      auto params = game_object_data.params;
      if (model_->IsGameObjectIdTaken(game_object_id)) {
        if (model_->IsLocalPlayerSet()
            && game_object_id == model_->GetLocalPlayer()->GetId()) {
          break;
        }
        // Interpolate position
        float x = params[0].toFloat();
        float y = params[1].toFloat();
        float new_x = x * time_to_interpolate / server_sent_time;
        float new_y = y * time_to_interpolate / server_sent_time;
        params[0] = new_x;
        params[1] = new_y;
        model_->GetGameObjectByGameObjectId(game_object_id)->SetParams(params);
      } else {
        model_->AddGameObject(game_object_id,
                              game_object_data.type, params);
      }
      bool previous_state
          = model_->GetGameObjectByGameObjectId(game_object_id)->IsInFov();
      model_->GetGameObjectByGameObjectId(game_object_id)->SetIsInFov(true);
      if (!previous_state) {
        qInfo() << "[CLIENT] Appeared in fov " << game_object_id;
      }
      game_objects_cache_[game_object_id].pop_front();
    }
  }
  this->UpdateLocalPlayer(delta_time);
  // this->TickPlayers(delta_time);
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

  ObjectCollision::MoveWithSlidingCollision(
      local_player, model_->GetAllGameObjects(),
      this->GetKeyForce(), delta_time);

  local_player->OnTick(delta_time);

  converter_->UpdateGameCenter(local_player->GetPosition());

  this->AddEventToSend(Event(EventType::kSendControls,
                        QDateTime::currentMSecsSinceEpoch() + time_difference_,
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
  this->AddEventToSend(Event(EventType::kSendGetVars));
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
  return;
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
    float rotation = Math::VectorAngle(local_player->GetPosition(),
                                       converter_->PointFromScreenToGame(
                                           mouse_event->pos()));
    local_player->SetRotation(rotation);
  }
}

// ------------------- GAME EVENTS -------------------

void ClientController::AddLocalPlayerGameObjectEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  model_->AddGameObject(game_object_id,
      GameObjectType::kPlayer,
      event.GetArgsSubVector(1));
  model_->GetGameObjectByGameObjectId(game_object_id)->SetIsInFov(true);
}

void ClientController::UpdateGameObjectDataEvent(const Event& event) {
  auto server_sent_time = event.GetArg<int64_t>(0);
  auto game_object_id = event.GetArg<GameObjectId>(1);
  auto game_object_type
      = static_cast<GameObjectType>(event.GetArg<int>(2));
  auto params = event.GetArgsSubVector(3);
  if (game_object_id != model_->GetLocalPlayer()->GetId()) {
    game_objects_cache_[game_object_id].push_back({server_sent_time, game_object_type,
                                                   params});
  }
}

void ClientController::GameObjectLeftFovEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  if (model_->IsGameObjectIdTaken(game_object_id)) {
    model_->GetGameObjectByGameObjectId(game_object_id)->SetIsInFov(false);
    qInfo() << "[CLIENT] Left from fov " << game_object_id;
    model_->DeleteGameObject(game_object_id);
  }
}
