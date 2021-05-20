#include "client_controller.h"

ClientController::ClientController(const QUrl& url)
    : model_(std::make_shared<ClientGameModel>()), url_(url) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  connect(&web_socket_, &QWebSocket::connected, this,
          &ClientController::OnConnected);
  connect(&web_socket_, &QWebSocket::disconnected, this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  connect(&shoot_check_timer, &QTimer::timeout, this,
          &ClientController::ShootHolding);
  shoot_check_timer.start(Constants::kShootHoldingCheck);
  this->StartTicking();
}

void ClientController::ConnectToRoom(RoomId room_id) {
  room_id = 1;  // temporary
  this->AddEventToSend(Event(EventType::kConnectToRoomById, room_id));
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

  Event time_event(EventType::kSetTimeDifference,
                   static_cast<qint64>(QDateTime::currentMSecsSinceEpoch()));
  this->SendEvent(time_event);
  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  Event event(message);
  // temp qInfo() << event;
  // Каждая миллисекунда важна для разницы времени,
  // так что не пропускаем через тик, а делаем сразу
  if (event.GetType() == EventType::kSetTimeDifference) {
    this->HandleEvent(event);
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
  this->UpdateInterpolationInfo();
  this->UpdateLocalPlayer(delta_time);
  this->UpdateLocalBullets(delta_time);
}

void ClientController::UpdateInterpolationInfo() {
  auto time_to_interpolate = GetCurrentServerTime()
      - Constants::kInterpolationMSecs;

  // Применяем запланированные на какое-то время обновления
  model_->UpdateScheduled(time_to_interpolate);
  // С учетом обновления булевской IsInFov
  // Удаляем объект из модели и из интерполятора
  for (const auto& game_object : model_->GetAllGameObjects()) {
    auto game_object_id = game_object->GetId();
    bool is_or_will_in_fov =
        model_->GetScheduledVariableValue(
            game_object_id, Variable::kIsInFov).toBool();
    if (!(is_or_will_in_fov || game_object->IsInFov())) {
      model_->DeleteGameObject(game_object_id);
      model_->RemoveFromInterpolator(game_object_id);
    }
  }

  // Интерполируем все, о чем есть информация
  for (const auto& [game_object_id, game_object_to_be_interpolated]
    : model_->GetInterpolatorMap()) {
    if (!model_->IsGameObjectIdTaken(game_object_id)) {
      model_->AttachGameObject(game_object_id,
                               game_object_to_be_interpolated->Clone());
    }
    auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
    Interpolator::InterpolateObject(game_object, game_object_to_be_interpolated,
                                    time_to_interpolate);
  }
}

void ClientController::UpdateLocalPlayer(int delta_time) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();

  std::vector<std::shared_ptr<GameObject>> game_objects_to_move_with_sliding;
  for (const auto& game_object : model_->GetAllGameObjects()) {
    if (game_object->GetType() != GameObjectType::kBullet) {
      game_objects_to_move_with_sliding.push_back(game_object);
    }
  }
  ObjectCollision::MoveWithSlidingCollision(
      local_player, game_objects_to_move_with_sliding,
      this->GetKeyForce(), delta_time);

  local_player->OnTick(delta_time);

  converter_->UpdateGameCenter(local_player->GetPosition());

  this->AddEventToSend(Event(EventType::kSendControls,
                        static_cast<qint64>(GetCurrentServerTime()),
                        local_player->GetId(),
                        local_player->GetX(),
                        local_player->GetY(),
                        local_player->GetVelocity(),
                        local_player->GetRotation()));
}

void ClientController::UpdateLocalBullets(int delta_time) {
  std::vector<GameObjectId> is_need_to_delete;
  for (const auto& bullet : model_->GetLocalBullets()) {
    auto object_collided = ObjectCollision::GetObjectBulletCollidedWith(
            bullet, model_->GetAllGameObjects(), delta_time, true);
    if (object_collided != nullptr) {
      bullet->SetIsNeedToDelete(true);
    }
    bullet->OnTick(delta_time);
    if (bullet->IsNeedToDelete()) {
      is_need_to_delete.push_back(bullet->GetId());
    }
  }

  for (const auto& bullet_id : is_need_to_delete) {
    model_->DeleteLocalBullet(bullet_id);
  }
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

void ClientController::SendVisibleRoomsInfoEvent(const Event& event) {
  model_->SetRoomsInfo(event.GetArg<QList<QVariant>>(0));
  view_->UpdateRoomsInfoList();
}

void ClientController::SetTimeDifferenceEvent(const Event& event) {
  auto client_sent_time = event.GetArg<int64_t>(0);
  auto server_received_time = event.GetArg<int64_t>(1);
  int64_t client_received_time = QDateTime::currentMSecsSinceEpoch();
  int64_t latency = (client_received_time - client_sent_time) / 2;
  time_difference_ = server_received_time - client_sent_time - latency;
  is_time_difference_set_ = true;
}

int64_t ClientController::GetCurrentServerTime() const {
  return BaseController::GetCurrentServerTime() + time_difference_;
}

// -------------------- CONTROLS --------------------

void ClientController::FocusOutEvent(QFocusEvent*) {
  for (const auto& [key, direction] : key_to_direction_) {
    is_direction_by_keys_[direction] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity({0, 0});
  }
  is_holding_ = false;
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

void ClientController::MousePressEvent(QMouseEvent*) {
  is_holding_ = true;
}

void ClientController::MouseReleaseEvent(QMouseEvent*) {
  is_holding_ = false;
}

void ClientController::ShootHolding() {
  if (!is_holding_) {
    return;
  }
  if (model_->IsLocalPlayerSet()) {
    auto local_player = model_->GetLocalPlayer();
    auto timestamp = GetCurrentServerTime();
    if (!local_player->GetWeapon()->IsPossibleToShoot(timestamp)) {
      return;
    }
    // Temporary nickname change
    this->AddEventToSend(Event(EventType::kSendNickname,
                               model_->GetLocalPlayer()->GetId(),
                               QString("Shooter#") +
                        QString::number(model_->GetLocalPlayer()->GetId())));
    local_player->GetWeapon()->SetLastTimeShot(timestamp);
    model_->AddLocalBullets();
    this->AddEventToSend(Event(EventType::kSendPlayerShooting,
                               static_cast<qint64>(GetCurrentServerTime()),
                               local_player->GetId()));
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
  qInfo() << event;
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto params = event.GetArgsSubVector(1);
  auto game_object =
      model_->GetGameObjectByGameObjectIdToBeInterpolated(game_object_id);
  game_object->SetParams(params);
  model_->AddScheduledUpdate(
      game_object_id, Variable::kIsInFov,
      {game_object->GetUpdatedTime(), true});
}

void ClientController::GameObjectLeftFovEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectInInterpolation(game_object_id)) {
    return;
  }
  auto game_object =
      model_->GetGameObjectByGameObjectIdToBeInterpolated(game_object_id);
  model_->AddScheduledUpdate(
      game_object_id, Variable::kIsInFov,
      {game_object->GetUpdatedTime(), false});
}

void ClientController::SendGameInfoToInterpolateEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto game_object_type = event.GetArg<GameObjectType>(1);
  auto sent_time = event.GetArg<int64_t>(2);
  auto event_type = event.GetArg<EventType>(3);
  sent_time = std::max(
      GetCurrentServerTime() - Constants::kInterpolationMSecs, sent_time);
  if (event_type == EventType::kUpdateGameObjectData) {
    model_->AddInterpolateInfo(game_object_id, game_object_type, sent_time);
  }
  auto args = event.GetArgsSubVector(4);
  this->HandleEvent(Event(event_type, args));
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  model_->DeleteGameObject(player_id);
  model_->RemoveFromInterpolator(player_id);
  game_state_ = GameState::kGameNotStarted;
  view_->Update();
}

void ClientController::UpdatePlayersStatsEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  model_->GetPlayerStatsByPlayerId(player_id)->SetParams(
      event.GetArgsSubVector(1));
  if (model_->IsGameObjectIdTaken(player_id)) {
    model_->GetPlayerByPlayerId(player_id)->SetLevel(
        model_->GetPlayerStatsByPlayerId(player_id)->GetLevel());
  }
}

void ClientController::UpdateLocalPlayerHealthPointsEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto health_points = event.GetArg<float>(0);
  model_->GetLocalPlayer()->SetHealthPoints(health_points);
}

void ClientController::LocalPlayerDiedEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto spawn_point = event.GetArg<QPointF>(0);
  model_->GetLocalPlayer()->Revive(spawn_point);
}

void ClientController::IncreaseLocalPlayerExperienceEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto experience_to_add = event.GetArg<float>(0);
  model_->GetLocalPlayer()->IncreaseExperience(experience_to_add);
}
