#include "client_controller.h"

ClientController::ClientController(const QUrl& url, int fps_max)
    : model_(std::make_shared<ClientGameModel>()),
      url_(url),
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
  this->StartTicking();
}

void ClientController::ConnectToRoom(RoomId room_id) {
  this->AddEventToSend(Event(EventType::kConnectToRoomById,
                             room_id, model_->GetLocalPlayerNickname()));
  controls_check_timer_.start(Constants::kControlsHoldingCheck);
}

std::shared_ptr<ClientGameModel> ClientController::GetModel() {
  return model_;
}

bool ClientController::IsGameInProgress() const {
  return game_state_ == GameState::kGameInProgress;
}

int64_t ClientController::GetHoldingRespawnButtonMsecs() const {
  return respawn_holding_current_;
}

int64_t ClientController::GetSecsToNextPossibleRevive() const {
  if (!model_->IsLocalPlayerSet()) {
    return -1;
  }
  if (model_->GetLocalPlayer()->IsAlive()) {
    auto delta_time =
        this->GetCurrentServerTime() - last_requested_respawn_time_;
    return (Constants::kRequestRespawnTime - delta_time) / 1000;
  } else {
    return (Constants::kReviveTime -
        (this->GetCurrentServerTime() - last_died_)) / 1000;
  }
}

bool ClientController::GetIsHoldingRespawnButton() const {
  if (!model_->IsLocalPlayerSet()) {
    return false;
  }
  if (model_->GetLocalPlayer()->IsAlive()) {
    return key_controller_->IsHeld(Controls::kRespawn);
  }
  return true;
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

void ClientController::SendPlayerDataToServer() {
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
  this->SendPlayerDataToServer();
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
    if (game_object->GetType() == GameObjectType::kPlayer) {
      std::dynamic_pointer_cast<Player>(game_object)->SetLevel(
          std::dynamic_pointer_cast<Player>(
              game_object_to_be_interpolated)->GetLevel());
    }
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

void ClientController::UpdateGameObjects() {
  for (const auto& object : model_->GetAllGameObjects()) {
    // Bullets always move, so we will only use animation state kIdle for them
    if (object->GetType() != GameObjectType::kBullet) {
      object->UpdateAnimationState();
    }
  }
}

void ClientController::SetView(std::shared_ptr<AbstractClientView> view) {
  view_ = std::move(view);
  converter_ = view_->GetConverter();
  key_controller_ = view_->GetKeyController();
}

void ClientController::UpdateView() {
  if (!is_time_difference_set_) {
    return;
  }
  auto time = QDateTime::currentMSecsSinceEpoch();
  if (last_view_update_time_ == -1) {
    last_view_update_time_ = time;
    return;
  }
  auto delta_time = time - last_view_update_time_;
  last_view_update_time_ = time;
  if (delta_time == 0) {
    return;
  }
  this->UpdateInterpolationInfo();
  this->UpdateLocalPlayer(delta_time);
  this->UpdateLocalBullets(delta_time);
  this->UpdateGameObjects();
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
  if (!view_) {
    return QVector2D();
  }
  bool is_up_pressed = key_controller_->IsHeld(Controls::kUp);
  bool is_right_pressed = key_controller_->IsHeld(Controls::kRight);
  bool is_down_pressed = key_controller_->IsHeld(Controls::kDown);
  bool is_left_pressed = key_controller_->IsHeld(Controls::kLeft);

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
  key_controller_->ClearControls();
  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity({0, 0});
  }
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  if (are_controls_blocked_) {
    return;
  }
  key_controller_->AddKeyReleaseEvent(key_event);
  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::KeyPressEvent(QKeyEvent* key_event) {
  if (are_controls_blocked_) {
    return;
  }
  key_controller_->AddKeyPressEvent(key_event);
  if (model_->IsLocalPlayerSet()) {
    model_->GetLocalPlayer()->SetVelocity(GetKeyForce());
  }
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {
  if (are_controls_blocked_) {
    return;
  }
  last_mouse_position_ = mouse_event->pos();
}

void ClientController::MousePressEvent(QMouseEvent* mouse_event) {
  key_controller_->AddMousePressEvent(mouse_event);
}

void ClientController::MouseReleaseEvent(QMouseEvent* mouse_event) {
  key_controller_->AddMouseReleaseEvent(mouse_event);
}

void ClientController::ControlsHolding() {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  if (are_controls_blocked_) {
    return;
  }

  if (key_controller_->IsHeld(Controls::kRespawn)) {
    if (respawn_holding_current_ >= Constants::kHoldingRespawnTime) {
      this->AddEventToSend(Event(EventType::kRequestRespawn,
                                 model_->GetLocalPlayerId()));
      last_requested_respawn_time_ = GetCurrentServerTime();
      respawn_holding_current_ = 0;
      are_controls_blocked_ = true;
    }
    if (this->GetCurrentServerTime() - last_requested_respawn_time_
        > Constants::kRequestRespawnTime) {
      respawn_holding_current_ += controls_check_timer_.interval();
    }
  } else  {
    respawn_holding_current_ = std::max(static_cast<int64_t>(0),
                respawn_holding_current_ - controls_check_timer_.interval());
  }

  if (key_controller_->WasPressed(ControlsWrapper::Controls::kReload)) {
    auto local_player_weapon = model_->GetLocalPlayer()->GetWeapon();
    auto timestamp = GetCurrentServerTime();
    if (local_player_weapon->IsPossibleToReload(timestamp)) {
      local_player_weapon->Reload(timestamp);
      this->AddEventToSend(Event(EventType::kSendPlayerReloading,
                                 static_cast<qint64>(timestamp),
                                 model_->GetLocalPlayerId()));
    }
    key_controller_->ClearKeyPress(ControlsWrapper::Controls::kReload);
  }

  if (key_controller_->IsHeld(Controls::kShoot)) {
    auto local_player = model_->GetLocalPlayer();
    auto timestamp = GetCurrentServerTime();

    // Reload if Bullets In Clips is empty
    if (local_player->GetWeapon()->GetCurrentBulletsInClip() <= 0 &&
        local_player->GetWeapon()->IsPossibleToReload(timestamp)) {
      local_player->UpdateAnimationState(true);
      local_player->GetWeapon()->Reload(timestamp);
      this->AddEventToSend(Event(EventType::kSendPlayerReloading,
                                 static_cast<qint64>(timestamp),
                                 local_player->GetId()));
    } else if (local_player->GetWeapon()->IsPossibleToShoot(timestamp)) {
      local_player->GetWeapon()->SetLastTimeShot(timestamp);

      QList<QVariant> bullet_shifts;
      static std::mt19937 generator_(QDateTime::currentMSecsSinceEpoch());
      // generate normalised accuracy
      std::uniform_real_distribution<> generate_shift_ =
          std::uniform_real_distribution<>(-1, 1);
      switch (local_player->GetWeapon()->GetWeaponType()) {
        case WeaponType::kShotgun: {
          bullet_shifts.push_back(generate_shift_(generator_));
          bullet_shifts.push_back(generate_shift_(generator_));
          bullet_shifts.push_back(generate_shift_(generator_));
          break;
        }

        case WeaponType::kAssaultRifle:
        case WeaponType::kCrossbow:
        case WeaponType::kMachineGun: {
          bullet_shifts.push_back(generate_shift_(generator_));
          break;
        }

        default: {
          qWarning() << "Invalid weapon type";
          break;
        }
      }
      local_player->GetWeapon()->SetCurrentBulletsInClip(
          local_player->GetWeapon()->GetCurrentBulletsInClip()
              - bullet_shifts.size());

      model_->AddLocalBullets(timestamp, bullet_shifts);
      this->AddEventToSend(Event(EventType::kSendPlayerShooting,
                                 static_cast<qint64>(timestamp),
                                 local_player->GetId(),
                                 bullet_shifts));
      local_player->SetAnimationState(AnimationState::kShoot, true);
    }
  } else {
    auto local_player = model_->GetLocalPlayer();
    if (local_player->GetAnimation()->GetState() == AnimationState::kShoot) {
      local_player->UpdateAnimationState(true);
    }
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
  if (model_->IsGameObjectIdTaken(game_object_id) &&
      model_->GetGameObjectByGameObjectId(game_object_id)->IsEntity()) {
    delete_time += Constants::kMaxDisappearTimeMSecs;
    std::shared_ptr<Entity> entity =
        std::dynamic_pointer_cast<Entity>(
            model_->GetGameObjectByGameObjectId(game_object_id));
    entity->SetDisappearing();
    entity->SetHealthPoints(0.f);
  }
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

void ClientController::PlayerRespawnedEvent(const Event& event) {
  view_->AddRespawnNotification(
      model_->GetPlayerStatsByPlayerId(
          event.GetArg<GameObjectId>(0))->GetNickname());
}

void ClientController::PlayerConnectedEvent(const Event& event) {
  view_->AddPlayerConnectedNotification(
      model_->GetPlayerStatsByPlayerId(
          event.GetArg<GameObjectId>(0))->GetNickname());
}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  view_->AddPlayerDisconnectedNotification(
      model_->GetPlayerStatsByPlayerId(player_id)->GetNickname());
  model_->DeleteGameObject(player_id);
  model_->DeletePlayerStats(player_id);
  model_->RemoveFromInterpolator(player_id);
  game_state_ = GameState::kGameNotStarted;
}

void ClientController::UpdatePlayersStatsEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  model_->GetPlayerStatsByPlayerId(player_id)->SetParams(
      event.GetArgsSubVector(1));
  if (model_->IsGameObjectInInterpolation(player_id)) {
    auto inter_player = std::dynamic_pointer_cast<Player>(
        model_->GetGameObjectByGameObjectIdToBeInterpolated(player_id));
    inter_player->SetLevel(
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

void ClientController::StartAttackAnimationEvent(const Event& event) {
  auto attacker_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectIdTaken(attacker_id)) {
    return;
  }
  if (attacker_id == model_->GetLocalPlayerId()) {
    return;
  }
  auto attacker = model_->GetGameObjectByGameObjectId(attacker_id);
  attacker->SetAnimationState(AnimationState::kAttack, true);
}

void ClientController::StartShootingAnimationEvent(const Event& event) {
  auto shooter_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectIdTaken(shooter_id)) {
    return;
  }
  if (shooter_id == model_->GetLocalPlayerId()) {
    return;
  }
  auto shooter = model_->GetGameObjectByGameObjectId(shooter_id);
  shooter->SetAnimationState(AnimationState::kShoot, true);
}

void ClientController::LocalPlayerDiedEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  key_controller_->ClearControls();
  last_died_ = this->GetCurrentServerTime();
  are_controls_blocked_ = true;
  last_requested_respawn_time_ = GetCurrentServerTime();
  model_->GetLocalPlayer()->SetHealthPoints(0.f);
}

void ClientController::ReviveLocalPlayerEvent(const Event& event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto local_player = model_->GetLocalPlayer();
  auto spawn_point = event.GetArg<QPointF>(0);
  local_player->Revive(spawn_point);
  last_requested_respawn_time_ = this->GetCurrentServerTime();
  this->AddEventToSend(Event(EventType::kReviveConfirmed,
                             local_player->GetId()));
  are_controls_blocked_ = false;
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
