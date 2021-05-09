#include "client_controller.h"

ClientController::ClientController(const QUrl& url,
                                   int fps_max) :
    url_(url),
    model_(std::make_shared<ClientGameModel>()),
    fps_max_(fps_max) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  qInfo() << "[CLIENT] Set fps_max to" << fps_max;
  connect(&web_socket_, &QWebSocket::connected, this,
          &ClientController::OnConnected);
  connect(&web_socket_, &QWebSocket::disconnected, this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  connect(&controls_check_timer_, &QTimer::timeout, this,
          &ClientController::ControlsHolding);
  controls_check_timer_.start(Constants::kControlsHoldingCheck);
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

  connect(&server_var_timer_, &QTimer::timeout,
          this, &ClientController::UpdateVarsAndPing);
  server_var_timer_.start(Constants::kTimeToUpdateVarsAndPing);
  connect(&view_update_timer_, &QTimer::timeout,
          this, &ClientController::UpdateView);
  view_update_timer_.start(1000 / fps_max_);
  connect(&web_socket_, &QWebSocket::pong,
          this, &ClientController::SetPing);

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
}

void ClientController::OnTickGameNotStarted(int delta_time) {
  // Temporary
  this->OnTickGameInProgress(delta_time);
}

void ClientController::SendPLayerDataToServer() {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto local_player = model_->GetLocalPlayer();
  this->AddEventToSend(Event(EventType::kSendControls,
                             static_cast<qint64>(GetCurrentServerTime()),
                             local_player->GetId(),
                             local_player->GetX(),
                             local_player->GetY(),
                             local_player->GetVelocity(),
                             local_player->GetRotation()));

  if (local_player->IsNeedToSendLevelingPoints()) {
    Event event(EventType::kSendLevelingPoints, local_player->GetId());
    for (const auto& item : local_player->GetLevelingPoints()) {
      event.PushBackArg(item);
    }
    this->AddEventToSend(event);
    local_player->SetNeedToSendLevelingPoints(false);
  }
}

void ClientController::OnTickGameInProgress(int delta_time) {
  this->SendPLayerDataToServer();
}

void ClientController::UpdateInterpolationInfo() {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto time_to_interpolate = GetCurrentServerTime()
      - Constants::kInterpolationMSecs;

  auto local_player = model_->GetLocalPlayer();
  std::unordered_map<GameObjectId, QPointF> old_objects_positions;
  for (const auto& game_object : model_->GetAllGameObjects()) {
    if (!model_->DoesObjectCollideByMoveWithSliding(game_object)) {
      continue;
    }
    if (ObjectCollision::AreCollided(local_player, game_object)) {
      continue;
    }
    old_objects_positions[game_object->GetId()] = game_object->GetPosition();
  }

  // Интерполируем все, о чем есть информация
  for (const auto&[game_object_id, game_object_to_be_interpolated]
    : model_->GetInterpolatorMap()) {
    if (!model_->IsGameObjectIdTaken(game_object_id)) {
      if (game_object_to_be_interpolated->GetCreatedTime() >=
        time_to_interpolate) {
        continue;
      }
      auto game_object = game_object_to_be_interpolated->Clone();
      model_->AttachGameObject(game_object_id, game_object);
      game_object->SetIsInterpolatedOnce(true);
      continue;
    }
    auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
    game_object->SetIsInterpolatedOnce(false);
    Interpolator::InterpolateObject(game_object, game_object_to_be_interpolated,
                                    time_to_interpolate);
  }

  int count_collisions_with_local_player = 0;
  QPointF player_position_offset;
  for (const auto&[game_object_id, prev_pos] : old_objects_positions) {
    auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
    auto new_pos = game_object->GetPosition();
    if (new_pos != prev_pos &&
      ObjectCollision::AreCollided(local_player, game_object)) {
      count_collisions_with_local_player++;
      player_position_offset = new_pos - prev_pos;
    }
  }
  if (count_collisions_with_local_player == 1) {
    local_player->SetPosition(
        local_player->GetPosition() + player_position_offset);
  }

  while (!time_to_delete_.empty()) {
    GameObjectId game_object_id = time_to_delete_.front().first;
    int64_t time = time_to_delete_.front().second;
    if (time >= time_to_interpolate) {
      break;
    }
    time_to_delete_.pop();
    if (model_->IsGameObjectIdTaken(game_object_id)) {
      model_->DeleteGameObject(game_object_id);
      model_->RemoveFromInterpolator(game_object_id);
    }
  }
}

void ClientController::UpdateLocalPlayer(int delta_time) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();

  float rotation = Math::VectorAngle(local_player->GetPosition() -
                                         view_->GetPlayerToCenterOffset(),
                                     converter_->PointFromScreenToGame(
                                         last_mouse_position_));
  local_player->SetRotation(rotation);

  std::vector<std::shared_ptr<GameObject>> game_objects_to_move_with_sliding =
      model_->GetGameObjectsToMoveWithSliding();
  ObjectCollision::MoveWithSlidingCollision(
      local_player, game_objects_to_move_with_sliding,
      this->GetKeyForce(), delta_time);

  local_player->OnTick(delta_time);

  converter_->UpdateGameCenter(local_player->GetPosition());
}

void ClientController::UpdateAnimations(int delta_time) {
  GameObject::GetAnimationsHolder().UpdateAnimations(delta_time);
}

void ClientController::UpdateLocalBullets(int delta_time) {
  std::vector<GameObjectId> is_need_to_delete;
  for (const auto& bullet : model_->GetLocalBullets()) {
    auto object_collided = ObjectCollision::GetObjectBulletCollidedWith(
            bullet, model_->GetAllExistGameObjects(),
            delta_time, true);
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

void ClientController::UpdateView() {
  auto time = QDateTime::currentMSecsSinceEpoch();
  auto delta_time = time - last_view_update_time_;
  last_view_update_time_ = time;
  if (delta_time == 0) {
    return;
  }
  this->UpdateInterpolationInfo();
  this->UpdateLocalPlayer(delta_time);
  this->UpdateLocalBullets(delta_time);
  this->UpdateAnimations(delta_time);
  view_->Update();
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
  if (is_controls_blocked_) {
    return;
  }
  for (const auto& [key, direction] : key_to_direction_) {
    is_direction_by_keys_[direction] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity({0, 0});
  }
  is_shoot_holding = false;
}

void ClientController::KeyPressEvent(QKeyEvent* key_event) {
  if (is_controls_blocked_) {
    return;
  }
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (native_key == Controls::kKeyC &&
    GetCurrentServerTime() - last_requested_respawn_time_
    > Constants::kRequestRespawnTime && !is_respawn_holding_) {
    started_holding_respawn_ = GetCurrentServerTime();
    is_respawn_holding_ = true;
    return;
  }
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = true;
  }

  model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  if (is_controls_blocked_) {
    return;
  }
  auto native_key = static_cast<Controls>(key_event->nativeScanCode());
  if (native_key == Controls::kKeyC) {
    respawn_released_time_ = GetCurrentServerTime();
    return;
  }
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]] = false;
  }

  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {
  if (is_controls_blocked_) {
    return;
  }
  last_mouse_position_ = mouse_event->pos();
}

void ClientController::MousePressEvent(QMouseEvent*) {
  if (is_controls_blocked_) {
    return;
  }
  is_shoot_holding = true;
}

void ClientController::MouseReleaseEvent(QMouseEvent*) {
  if (is_controls_blocked_) {
    return;
  }
  is_shoot_holding = false;
}

void ClientController::ControlsHolding() {
  if (is_respawn_holding_) {
    if (GetCurrentServerTime() - respawn_released_time_ > 50) {
      is_respawn_holding_ = false;
    }
  }
  if (is_respawn_holding_) {
    if (GetCurrentServerTime() - started_holding_respawn_
        > Constants::kHoldingRespawnTime) {
      this->AddEventToSend(Event(EventType::kRequestRespawn,
                                 model_->GetLocalPlayer()->GetId()));
      last_requested_respawn_time_ = GetCurrentServerTime();
      started_holding_respawn_ = GetCurrentServerTime();
      is_respawn_holding_ = false;
      is_controls_blocked_ = true;
    }
    return;
  }
  if (is_shoot_holding) {
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
                                     QString::number(local_player->GetId())));
      local_player->GetWeapon()->SetLastTimeShot(timestamp);
      model_->AddLocalBullets(timestamp);
      this->AddEventToSend(Event(EventType::kSendPlayerShooting,
                                 static_cast<qint64>(timestamp),
                                 local_player->GetId()));
    }
    return;
  }
}

// ------------------- GAME EVENTS -------------------

void ClientController::AddLocalPlayerGameObjectEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  model_->AddGameObject(game_object_id,
      GameObjectType::kPlayer,
      event.GetArgsSubVector(1));
}

void ClientController::UpdateGameObjectDataEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto params = event.GetArgsSubVector(1);
  auto game_object =
      model_->GetGameObjectByGameObjectIdToBeInterpolated(game_object_id);
  game_object->SetParams(params);
}

void ClientController::DeleteGameObjectEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto delete_time = event.GetArg<int64_t>(1);
  time_to_delete_.push({game_object_id, delete_time});
}

void ClientController::SendGameInfoToInterpolateEvent(const Event& event) {
  auto game_object_id = event.GetArg<GameObjectId>(0);
  auto game_object_type = event.GetArg<GameObjectType>(1);
  auto sent_time = event.GetArg<int64_t>(2);
  auto event_type = event.GetArg<EventType>(3);
  sent_time = std::max(
      GetCurrentServerTime() - Constants::kInterpolationMSecs, sent_time);
  auto args = event.GetArgsSubVector(4);
  if (event_type == EventType::kUpdateGameObjectData) {
    model_->AddInterpolateInfo(game_object_id, game_object_type, sent_time);
  } else if (event_type == EventType::kDeleteGameObject) {
    args.emplace_back(static_cast<qint64>(sent_time));
  }
  this->HandleEvent(Event(event_type, args));
}

void ClientController::PlayerKilledNotificationEvent(const Event& event) {
  auto victim_id = event.GetArg<GameObjectId>(0);
  auto killer_id = event.GetArg<GameObjectId>(1);
  auto weapon_type = event.GetArg<WeaponType>(2);

  auto killer_name = this->GetEntityName(killer_id);
  auto victim_name = this->GetEntityName(victim_id);
  view_->AddKillFeedNotification(killer_name, victim_name, weapon_type);
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  model_->DeleteGameObject(player_id);
  model_->DeletePlayerStats(player_id);
  model_->RemoveFromInterpolator(player_id);
  game_state_ = GameState::kGameNotStarted;
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
  last_requested_respawn_time_ = GetCurrentServerTime();
  model_->GetLocalPlayer()->SetHealthPoints(health_points);
}

void ClientController::LocalPlayerDiedEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  is_controls_blocked_ = true;
  is_shoot_holding = false;
  last_requested_respawn_time_ = GetCurrentServerTime();
  model_->GetLocalPlayer()->SetIsVisible(false);
}

void ClientController::ReviveLocalPlayerEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto local_player = model_->GetLocalPlayer();
  auto spawn_point = event.GetArg<QPointF>(0);
  local_player->Revive(spawn_point);
  local_player->SetIsVisible(true);
  this->AddEventToSend(Event(EventType::kReviveConfirmed,
                             local_player->GetId()));
  is_controls_blocked_ = false;
}

void ClientController::IncreaseLocalPlayerExperienceEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto experience_to_add = event.GetArg<float>(0);
  model_->GetLocalPlayer()->IncreaseExperience(experience_to_add);
}

void ClientController::ShootFailedEvent(const Event& event) {
  auto timestamp = static_cast<int64_t>(event.GetArg<qint64>(0));
  for (const auto& bullet : model_->GetLocalBullets()) {
    if (bullet->GetUpdatedTime() == timestamp) {
      bullet->SetIsNeedToDelete(true);
    }
  }
}

QString ClientController::GetEntityName(GameObjectId game_object_id) const {
  auto killer_object_type =
      model_->GetGameObjectByGameObjectId(game_object_id)->GetType();
  if (killer_object_type == GameObjectType::kPlayer) {
    return model_->GetPlayerStatsByPlayerId(game_object_id)->GetNickname();
  } else {
    return "Creep";
  }
}
