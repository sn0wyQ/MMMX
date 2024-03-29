#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), model_(std::make_shared<RoomGameModel>()),
      room_settings_(room_settings) {
  this->StartTicking();
  this->AddCreeps();
  this->AddConstantObjects();
}

void RoomController::AddEventToSendToSingleClient(const Event& event,
                                                  ClientId client_id) {
  Event event_to_send(EventType::kSendEventToClient, client_id);

  event_to_send.PushBackArg(static_cast<int>(event.GetType()));
  auto args = event.GetArgs();
  for (const auto& arg : args) {
    event_to_send.PushBackArg(arg);
  }

  this->BaseController::AddEventToSend(event_to_send);
}

void RoomController::AddEventToSendToClientList(
    const Event& event,
    const std::vector<ClientId>& client_ids) {
  for (ClientId client_id : client_ids) {
    this->AddEventToSendToSingleClient(event, client_id);
  }
}

void RoomController::AddEventToSendToAllClients(const Event& event) {
  this->AddEventToSendToClientList(event, this->GetAllClientsIds());
}

void RoomController::AddEventToSendToSinglePlayer(const Event& event,
                                                  GameObjectId player_id) {
  try {
    ClientId client_id = this->PlayerIdToClientId(player_id);
    this->AddEventToSendToSingleClient(event, client_id);
  } catch (std::exception& e) {
    qWarning() << e.what() << "Maybe original player already disconnected?";
  }
}

void RoomController::AddEventToSendToPlayerList(
    const Event& event, const std::vector<GameObjectId>& player_ids) {
  for (GameObjectId player_id : player_ids) {
    this->AddEventToSendToSinglePlayer(event, player_id);
  }
}

void RoomController::AddEventToSendToAllPlayers(const Event& event) {
  this->AddEventToSendToPlayerList(event, this->GetAllPlayerIds());
}

void RoomController::SendEvent(const Event& event) {
  this->BaseController::LogEvent(event);
  events_for_server_.push_back(event);
}

void RoomController::OnTick(int delta_time) {
  models_cache_.push_back({delta_time,
                           std::make_shared<RoomGameModel>(*model_)});
  model_ = models_cache_.back().model;
  this->RecalculateModel(models_cache_.back());
  this->AddCreeps();
  for (const auto& player_id : this->GetAllPlayerIds()) {
    // Рассказываем НАМ о других
    SendGameObjectsDataToPlayer(player_id);
  }
  this->SendPlayersStatsToPlayers();
  model_->UpdatePlayerStatsHashes();
  model_->UpdateGameObjectHashes();
  if (static_cast<int>(models_cache_.size()) > Constants::kTicksToStore) {
    models_cache_.pop_front();
  }
}

void RoomController::RecalculateModel(const ModelData& model_data) {
  this->ProcessBulletsHits(model_data);
  this->TickCreepsIntelligence(model_data);
  this->TickObjectsInModel(model_data);
  this->DeleteReadyToBeDeletedObjects(model_data);
  this->RevivePlayers(model_data);
}

void RoomController::RevivePlayers(
    const RoomController::ModelData& model_data) {
  while (!revive_player_at_.empty()) {
    int64_t revive_at = revive_player_at_.front().second;
    if (revive_at <= GetCurrentServerTime()) {
      auto player_id = revive_player_at_.front().first;
      revive_player_at_.pop();
      if (!model_data.model->IsGameObjectIdTaken(player_id)) {
        continue;
      }
      auto player = model_data.model->GetPlayerByPlayerId(player_id);
      QPointF point_to_spawn =
          model_->GetPointToSpawn(player->GetRigidBodyBoundingCircleRadius(),
                                  GameObjectType::kPlayer);
      player->Revive(point_to_spawn);
      this->AddEventToSendToSinglePlayer(
          Event(EventType::kReviveLocalPlayer, point_to_spawn),
          player_id);
    } else {
      break;
    }
  }
}

void RoomController::DeleteReadyToBeDeletedObjects(
    const ModelData& model_data) {
  auto game_objects = model_data.model->GetAllGameObjects();
  for (const auto& game_object : game_objects) {
    if (game_object->IsNeedToDelete()) {
      this->AddEventToSendToAllPlayers(
          GetEventOfDeleteGameObject(game_object->GetId()));
      model_data.model->DeleteGameObject(game_object->GetId());
    }
  }
}

void RoomController::TickObjectsInModel(const ModelData& model_data) {
  auto game_objects = model_data.model->GetAllGameObjects();
  for (const auto& game_object : game_objects) {
    game_object->OnTick(model_data.delta_time);
  }
}

void RoomController::EntityReceiveDamage(const ModelData& model_data,
                                         const std::shared_ptr<Entity>& killer,
                                         const std::shared_ptr<Entity>& entity,
                                         float damage, bool* is_killed) {
  if (!entity->IsAlive()) {
    *is_killed = false;
    return;
  }
  float cur_entity_hp = entity->GetHealthPoints();
  float hp_to_set = std::max(0.f, cur_entity_hp - damage);
  bool is_player = entity->GetType() == GameObjectType::kPlayer;
  bool is_creep = entity->GetType() == GameObjectType::kCreep;
  entity->SetHealthPoints(hp_to_set);
  if (hp_to_set == 0.f) {
    if (killer && killer->GetType() == GameObjectType::kPlayer) {
      auto killer_player = std::dynamic_pointer_cast<Player>(killer);
      float receive_exp = entity->GetExpIncrementForKill();
      killer_player->IncreaseExperience(receive_exp);
      auto killer_stats =
          model_data.model->GetPlayerStatsByPlayerId(killer->GetId());
      if (is_player) {
        killer_stats->SetKills(killer_stats->GetKills() + 1);
      }
      killer_stats->SetLevel(killer->GetLevel());
      this->AddEventToSendToSinglePlayer(
          Event(EventType::kIncreaseLocalPlayerExperience,
                receive_exp), killer->GetId());
    }
    if (is_player) {
      model_data.model->GetPlayerStatsByPlayerId(
          entity->GetId())->GetMutableDeaths()++;
      this->AddEventToSendToSinglePlayer(
          Event(EventType::kLocalPlayerDied),
          entity->GetId());
      revive_player_at_.push({entity->GetId(),
                              GetCurrentServerTime() + Constants::kReviveTime});
      are_controls_blocked_[entity->GetId()] = true;
    } else if (is_creep) {
      --creeps_count_;
      entity->SetIsNeedToDelete(true);
    }
    *is_killed = true;
  } else {
    *is_killed = false;
  }
  if (is_player) {
    this->AddEventToSendToSinglePlayer(
        Event(EventType::kUpdateLocalPlayerHealthPoints, hp_to_set),
        entity->GetId());
  }
}

void RoomController::TickCreepsIntelligence(
    const RoomController::ModelData& model_data) {
  auto creeps = model_data.model->GetCreeps();
  auto players = model_data.model->GetAlivePlayers();
  for (auto& creep : creeps) {
    creep->TickIntelligence(model_->GetAllGameObjects());
    ObjectCollision::MoveWithSlidingCollision(
        creep, model_->GetGameObjectsToMoveWithSliding(),
        creep->GetForce(), model_data.delta_time);
    auto timestamp = GetCurrentServerTime();
    if (creep->IsPossibleToAttack(timestamp)) {
      auto player_id_to_damage = creep->GetPlayerToDamage(players);
      if (player_id_to_damage != Constants::kNullGameObjectId) {
        auto player = model_->GetPlayerByPlayerId(player_id_to_damage);
        AddEventToSendToAllPlayers(Event(EventType::kStartAttackAnimation,
                                         creep->GetId()));
        bool is_killed;
        EntityReceiveDamage(model_data, creep,
                            player, creep->GetDamage(),
                            &is_killed);
        if (is_killed) {
          this->AddEventToSendToAllPlayers(
              Event(EventType::kPlayerKilledNotification,
                    player->GetId(), creep->GetId(),
                    static_cast<int>(WeaponType::kNone)));
        }
        creep->SetLastAttackedTime(timestamp);
      }
    }
  }
}

void RoomController::ProcessBulletHits(
    const RoomController::ModelData& model_data_bullet,
    const std::shared_ptr<Bullet>& bullet,
    const std::vector<std::shared_ptr<GameObject>>& game_objects) {
  auto old_object_collided =
      ObjectCollision::GetObjectBulletCollidedWith(
          bullet, game_objects, model_data_bullet.delta_time, false);
  if (old_object_collided) {
    bullet->SetIsNeedToDelete(true);

    auto object_collided_id = old_object_collided->GetId();
    if (!model_data_bullet.model->IsGameObjectIdTaken(object_collided_id)) {
      return;
    }
    auto actual_object_collided =
        model_data_bullet.model->GetGameObjectByGameObjectId(
            object_collided_id);
    if (actual_object_collided->IsEntity() &&
        actual_object_collided->IsAlive()) {
      auto injured = std::dynamic_pointer_cast<Entity>(actual_object_collided);
      bool is_killed;
      auto damager =
         model_->IsGameObjectIdTaken(bullet->GetParentId()) ?
          std::dynamic_pointer_cast<Entity>(
              model_->GetGameObjectByGameObjectId(
                  bullet->GetParentId())) : nullptr;
      EntityReceiveDamage(model_data_bullet,
                          damager, injured,
                          bullet->GetBulletDamage(), &is_killed);
      if (injured->GetType() == GameObjectType::kCreep &&
          damager->GetType() == GameObjectType::kPlayer) {
        std::dynamic_pointer_cast<Creep>(injured)->SetAggressivePlayer(
            std::dynamic_pointer_cast<Player>(damager));
      }

      if (is_killed && injured->GetType() == GameObjectType::kPlayer) {
        this->AddEventToSendToAllPlayers(
            Event(EventType::kPlayerKilledNotification,
                  injured->GetId(),
                  bullet->GetParentId(),
                  static_cast<int>(std::dynamic_pointer_cast<Player>(damager)->
                      GetWeapon()->GetWeaponType())));
      }
    }
  }
}

void RoomController::ProcessBulletsHits(const ModelData& model_data) {
  auto timestamp = GetCurrentServerTime() - Constants::kInterpolationMSecs;
  auto model_id = GetModelIdByTimestamp(timestamp);
  if (model_id < 0) {
    return;
  }
  auto game_objects =
      models_cache_[model_id].model->GetAllExistGameObjects();
  auto bullets = model_data.model->GetAllBullets();
  for (const auto& bullet : bullets) {
    this->ProcessBulletHits(model_data, bullet, game_objects);
  }
}

void RoomController::AddClient(
    ClientId client_id, const QString& nickname, PlayerType player_type) {
  GameObjectId player_id = AddPlayer(player_type);
  player_ids_[client_id] = player_id;
  are_controls_blocked_[player_id] = false;

  Event event_add_local_player(EventType::kAddLocalPlayerGameObject, player_id);
  event_add_local_player.PushBackArgs(
      model_->GetGameObjectByGameObjectId(player_id)->GetParams());
  this->AddEventToSendToSinglePlayer(event_add_local_player, player_id);
  this->AddEventToSendToSingleClient(
      Event(EventType::kSetPlayerIdToClient, player_id), client_id);
  auto player = model_->GetPlayerByPlayerId(player_id);
  model_->AddPlayerStats(player_id,
                         nickname,
                         player->GetLevel());
  this->SendGameObjectsDataToPlayer(player_id, true);
  this->ForceSendPlayersStatsToPlayer(player_id);
  this->SendPlayersStatsToPlayers();
  this->AddEventToSendToAllPlayers(Event(EventType::kPlayerConnected,
                                         player_id));

  qInfo().noquote().nospace() << "[ROOM ID: " << id_
                              << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToSendToAllClients(Event(EventType::kStartGame));
    room_state_ = RoomState::kGameInProgress;
    qInfo().noquote().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
}

void RoomController::RemoveClient(ClientId client_id) {
  GameObjectId player_id = ClientIdToPlayerId(client_id);
  if (player_id == Constants::kNullClientId) {
    throw std::runtime_error(
        "[ROOM ID:" + std::to_string(id_) + "] Invalid client ID");
  }
  are_controls_blocked_.erase(player_id);
  model_->DeleteGameObject(player_id);
  model_->DeletePlayerStats(player_id);
  this->AddEventToSendToAllClients(Event(EventType::kPlayerDisconnected,
                                         player_id));
  player_ids_.erase(client_id);
  room_state_ = (this->GetPlayersCount()
                 ? RoomState::kWaitingForClients : RoomState::kGameFinished);
  qInfo().noquote().nospace() << "[ROOM ID: " << id_
                              << "] Removed Player ID: " << player_id;
}

bool RoomController::HasFreeSpot() const {
  return this->GetPlayersCount() < room_settings_.GetMaxClients();
}

bool RoomController::HasPlayers() const {
  return this->GetPlayersCount() > 0;
}

bool RoomController::IsGameInProgress() const {
  return room_state_ == RoomState::kGameInProgress;
}

bool RoomController::IsWaitingForClients() const {
  return room_state_ == RoomState::kWaitingForClients;
}

bool RoomController::IsPublic() const {
  return room_settings_.IsPublic();
}

void RoomController::SetRoomState(RoomState room_state) {
  room_state_ = room_state;
}

void RoomController::StartGame() {
  this->SetRoomState(RoomState::kGameInProgress);
  this->AddEventToHandle(Event(EventType::kStartGame));
}

int RoomController::GetPlayersCount() const {
  return static_cast<int>(player_ids_.size());
}

std::vector<ClientId> RoomController::GetAllClientsIds() const {
  std::vector<ClientId> result;
  for (auto[client_id, player_id] : player_ids_) {
    result.push_back(client_id);
  }
  return result;
}

std::vector<GameObjectId> RoomController::GetAllPlayerIds() const {
  std::vector<GameObjectId> result;
  for (auto[client_id, player_id] : player_ids_) {
    result.push_back(player_id);
  }
  return result;
}

RoomId RoomController::GetId() const {
  return id_;
}

GameObjectId RoomController::ClientIdToPlayerId(ClientId client_id) const {
  auto iter = player_ids_.find(client_id);
  if (iter == player_ids_.end()) {
    return Constants::kNullGameObjectId;
  }
  return iter->second;
}

QString RoomController::GetControllerName() const {
  return "ROOM ID: " + QString::number(id_);
}

ClientId RoomController::PlayerIdToClientId(GameObjectId player_id) const {
  for (auto[client_id, player_id_in] : player_ids_) {
    if (player_id == player_id_in) {
      return client_id;
    }
  }
  throw std::runtime_error("[ROOM] No such player");
}

std::vector<Event> RoomController::ClaimEventsForServer() {
  return std::move(events_for_server_);
}

RoomInfo RoomController::GetRoomInfo() const {
  RoomInfo room_info;
  room_info.id = id_;
  room_info.name = room_settings_.GetName();
  room_info.max_clients = room_settings_.GetMaxClients();
  room_info.owners_client_id = room_settings_.GetOwnersClientId();
  room_info.is_public = room_settings_.IsPublic();
  for (const auto& [client_id, player_id] : player_ids_) {
    room_info.clients.push_back(client_id);
  }
  return room_info;
}

Event RoomController::GetEventOfGameObjectData(
    GameObjectId game_object_id) const {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  Event event(EventType::kSendGameInfoToInterpolate,
              game_object_id,
              static_cast<int>(game_object->GetType()),
              static_cast<qint64>(GetCurrentServerTime()),
              static_cast<int>(EventType::kUpdateGameObjectData),
              game_object_id);
  event.PushBackArgs(game_object->GetParams());
  return event;
}

Event RoomController::GetEventOfDeleteGameObject(
    GameObjectId game_object_id) const {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  Event event(EventType::kSendGameInfoToInterpolate,
              game_object_id,
              static_cast<int>(game_object->GetType()),
              static_cast<qint64>(GetCurrentServerTime()),
              static_cast<int>(EventType::kDeleteGameObject),
              game_object_id);
  return event;
}

void RoomController::SendGameObjectsDataToPlayer(GameObjectId player_id,
                                                 bool force_sending) {
  for (const auto& object : model_->GetAllGameObjects()) {
    if (object->GetId() == player_id) {
      continue;
    }

    if (object->GetType() == GameObjectType::kBullet) {
      auto bullet = std::dynamic_pointer_cast<Bullet>(object);
      if (bullet->GetParentId() == player_id) {
        continue;
      }
    }

    if (force_sending ||
      model_->IsNeededToSendGameObjectData(object->GetId())) {
      this->AddEventToSendToSinglePlayer(
          GetEventOfGameObjectData(object->GetId()), player_id);
    }
  }
}

void RoomController::ForceSendPlayersStatsToPlayer(GameObjectId player_id) {
  for (auto player_stats_id : this->GetAllPlayerIds()) {
    Event event_update_players_stats = Event(EventType::kUpdatePlayersStats,
                                             player_stats_id);
    event_update_players_stats.PushBackArgs(
        model_->GetPlayerStatsByPlayerId(player_stats_id)->GetParams());
    this->AddEventToSendToSinglePlayer(event_update_players_stats, player_id);
  }
}

void RoomController::SendPlayersStatsToPlayers() {
  for (auto player_id : this->GetAllPlayerIds()) {
    if (model_->IsNeededToSendPlayerStats(player_id)) {
      Event event_update_players_stats = Event(EventType::kUpdatePlayersStats,
                                               player_id);
      event_update_players_stats.PushBackArgs(
          model_->GetPlayerStatsByPlayerId(player_id)->GetParams());
      this->AddEventToSendToAllPlayers(event_update_players_stats);
    }
  }
}

GameObjectId RoomController::AddPlayer(PlayerType player_type) {
  QPointF point = model_->GetPointToSpawn(Constants::kDefaultPlayerRadius,
                                          GameObjectType::kPlayer);

  AnimationType animation_type = AnimationType::kNone;
  WeaponType weapon_type = WeaponType::kNone;
  float width = 8.f;
  float height = 8.f;

  switch (player_type) {
    case PlayerType::kSmasher: {
      animation_type = AnimationType::kSmasher;
      weapon_type = WeaponType::kShotgun;
      break;
    }

    case PlayerType::kSoldier: {
      animation_type = AnimationType::kSoldier;
      weapon_type = WeaponType::kAssaultRifle;
      break;
    }

    case PlayerType::kSpider: {
      animation_type = AnimationType::kSpider;
      weapon_type = WeaponType::kMachineGun;
      width = 3.f;
      height = 3.f;
      break;
    }

    case PlayerType::kViking: {
      animation_type = AnimationType::kViking;
      weapon_type = WeaponType::kCrossbow;
      break;
    }

    default: {
      qWarning() << "Invalid player type";
      break;
    }
  }

  std::vector<QVariant>
      params = {point.x(),
                point.y(),
                Constants::kDefaultPlayerRotation,
                width, height,
                static_cast<int>(RigidBodyType::kCircle),
                Constants::kDefaultPlayerRadius * 2.f,
                Constants::kDefaultPlayerRadius * 2.f,
                static_cast<int>(animation_type),
                0.f, 0.f, Constants::kDefaultSpeedMultiplier,
                Constants::kDefaultEntityFov * 2.f,
                Constants::kDefaultMaxHealthPoints,
                Constants::kDefaultHealthRegenSpeed,
                Constants::kDefaultMaxHealthPoints,
                static_cast<int>(weapon_type)};

  return model_->AddGameObject(GameObjectType::kPlayer, params);
}

void RoomController::AddGarage(float x, float y, float rotation,
                               float width, float height) {
  model_->AddGameObject(GameObjectType::kGameObject,
                        {x, y, rotation, width, height,
                         static_cast<int>(RigidBodyType::kRectangle),
                         width * 0.85f, height * 0.97f,
                         static_cast<int>(AnimationType::kGarage)});
}

void RoomController::AddRandomGarage(float width, float height) {
  QPointF position = model_->GetPointToSpawn(
      Math::DistanceBetweenPoints(QPointF(),
                                  QPointF(width / 2.f, height / 2.f)),
      GameObjectType::kGameObject);
  static std::mt19937 rng(QDateTime::currentMSecsSinceEpoch());
  std::uniform_real_distribution<> random_rotation(0, 360);
  AddGarage(position.x(), position.y(), random_rotation(rng), width, height);
}

void RoomController::AddTree(float x, float y, float radius) {
  model_->AddGameObject(GameObjectType::kGameObject,
                        {x, y, 0.f, radius * 2.f, radius * 2.f,
                         static_cast<int>(RigidBodyType::kCircle),
                         radius * 1.45f, radius * 1.45f,
                         static_cast<int>(AnimationType::kTreeGreen)});
}

void RoomController::AddRandomTree(float radius) {
  QPointF position = model_->GetPointToSpawn(radius,
                                             GameObjectType::kGameObject);
  AddTree(position.x(), position.y(), radius);
}

void RoomController::AddCreep(float x, float y) {
  float distance = QLineF(QPointF(), QPointF(x, y)).length();
  auto params = CreepSettings::GetInstance().GetCreepParams(x, y, distance);
  if (params.empty()) {
    qWarning() << "[ROOM CONTROLLER] No suitable creep found";
    return;
  }
  auto game_object_id =
      model_->AddGameObject(GameObjectType::kCreep, params);
  auto creep = std::dynamic_pointer_cast<Creep>(
      model_->GetGameObjectByGameObjectId(game_object_id));
  CreepSettings::SetStaticParams(creep);
}

std::vector<GameObjectId> RoomController::AddBullets(
    const std::shared_ptr<RoomGameModel>& model, GameObjectId parent_id,
                               float x, float y, float rotation,
                               const std::shared_ptr<Weapon>& weapon,
                               const QList<QVariant>& random_bullet_shifts) {
  std::vector<std::vector<QVariant>> bullets_params =
      weapon->GetBulletsParams(parent_id, x, y, rotation, random_bullet_shifts);
  std::vector<GameObjectId> bullet_ids;
  for (const std::vector<QVariant>& bullet_params : bullets_params) {
    int bullet_id = model_->GenerateNextUnusedBulletId(parent_id);
    model->GameModel::AddGameObject(bullet_id,
        GameObjectType::kBullet,
        bullet_params);
    bullet_ids.emplace_back(bullet_id);
  }
  return bullet_ids;
}

void RoomController::AddConstantObjects() {
  model_->AddGameObject(GameObjectType::kMapBorder,
                        {0.f, 0.f, 0.f,
                         Constants::kMapPictureWidth,
                         Constants::kMapPictureHeight,
                         static_cast<int>(RigidBodyType::kRectangle),
                         Constants::kMapWidth,
                         Constants::kMapHeight,
                         static_cast<int>(AnimationType::kMap)});

  for (int i = 0; i < 10; i++) {
    this->AddRandomGarage(10.f, 7.772f);
  }
  for (int i = 0; i < 20; i++) {
    this->AddRandomTree(4.f);
  }
}

void RoomController::AddCreeps() {
  for (; creeps_count_ < 10; creeps_count_++) {
    QPointF position = model_->GetPointToSpawn(
        Math::DistanceBetweenPoints(QPointF(), QPointF(
        CreepSettings::GetInstance().GetMaxCreepSize().height(),
        CreepSettings::GetInstance().GetMaxCreepSize().width())),
        GameObjectType::kCreep);
    this->AddCreep(position.x(), position.y());
  }
}

int RoomController::GetModelIdByTimestamp(int64_t timestamp) const {
  int64_t latency = GetCurrentServerTime() - timestamp;
  latency = std::max(static_cast<int64_t>(0), latency);
  int latency_in_ticks = static_cast<int>(latency / Constants::kTimeToTick);
  return static_cast<int>(models_cache_.size()) - 1 - latency_in_ticks;
}

void RoomController::SendNicknameEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  auto nickname = event.GetArg<QString>(1);
  model_->GetPlayerStatsByPlayerId(player_id)->SetNickname(nickname);
}

// ------------------- GAME EVENTS -------------------

void RoomController::SendControlsEvent(const Event& event) {
  auto timestamp = event.GetArg<int64_t>(0);
  auto model_id = GetModelIdByTimestamp(timestamp);
  // Проигнорим, если чел нам прислал то, что он сделал очень давно
  if (model_id < 0) {
    return;
  }
  auto current_model_data = models_cache_[model_id];
  auto player_id = event.GetArg<GameObjectId>(1);
  if (are_controls_blocked_[player_id]) {
    return;
  }
  if (!current_model_data.model->IsGameObjectIdTaken(player_id)) {
    return;
  }
  // Мы перемещаем человека в той самой модели из прошлого
  QPointF position_to_set =
      {event.GetArg<float>(2), event.GetArg<float>(3)};
  auto velocity = event.GetArg<QVector2D>(4);
  auto rotation = event.GetArg<float>(5);
  // А теперь с учетом этого проталкиваем его пересечение на будущее
  // учитывая velocity и rotation
  while (model_id != static_cast<int>(models_cache_.size())) {
    auto cur_model = models_cache_[model_id].model;
    if (!cur_model->IsGameObjectIdTaken(player_id)) {
      break;
    }

    auto player_in_model
        = cur_model->GetPlayerByPlayerId(player_id);
    player_in_model->SetPosition(position_to_set);
    player_in_model->SetVelocity(velocity);
    player_in_model->SetRotation(rotation);
    player_in_model->OnTick(models_cache_[model_id].delta_time);
    position_to_set = player_in_model->GetPosition();
    model_id++;
  }
}

void RoomController::SendLevelingPointsEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectIdTaken(player_id)) {
    return;
  }
  auto player = model_->GetPlayerByPlayerId(player_id);
  std::vector<int> leveling_points;
  for (int i = 0; i < static_cast<int>(LevelingSlots::SIZE); i++) {
    auto param = event.GetArg<int>(1 + i);
    leveling_points.push_back(param);
  }
  auto prev_leveling_points = player->GetLevelingPoints();
  for (int i = 0; i < static_cast<int>(LevelingSlots::SIZE); i++) {
    while (prev_leveling_points[i] < leveling_points[i]) {
      player->IncreaseLevelingPoint(static_cast<LevelingSlots>(i));
      prev_leveling_points[i]++;
    }
  }
}

void RoomController::SendPlayerReloadingEvent(const Event& event) {
  auto timestamp = event.GetArg<int64_t>(0);
  auto model_id = GetModelIdByTimestamp(timestamp);
  // Проигнорим, если чел нам прислал то, что он сделал очень давно
  if (model_id < 0) {
    return;
  }
  auto current_model_data = models_cache_[model_id];
  auto player_id = event.GetArg<GameObjectId>(1);

  if (!current_model_data.model->IsGameObjectIdTaken(player_id) ||
      are_controls_blocked_[player_id]) {
    return;
  }

  std::vector<GameObjectId> players_ids = this->GetAllPlayerIds();
  for (size_t i = 0; i < players_ids.size(); i++) {
    if (player_id == players_ids[i]) {
      players_ids.erase(players_ids.begin() + i);
      break;
    }
  }
  this->AddEventToSendToPlayerList(Event(EventType::kSendPlayerReloading,
                      static_cast<qint64>(timestamp), player_id), players_ids);
}

void RoomController::SendPlayerShootingEvent(const Event& event) {
  auto timestamp = event.GetArg<int64_t>(0);
  auto model_id = GetModelIdByTimestamp(timestamp);
  auto player_id = event.GetArg<GameObjectId>(1);
  if (model_id < 0) {
    this->AddEventToSendToSinglePlayer(
        Event(EventType::kShootFailed,
              static_cast<qint64>(timestamp)), player_id);
    return;
  }
  auto start_model = models_cache_[model_id].model;
  if (!start_model->IsGameObjectIdTaken(player_id) ||
      are_controls_blocked_[player_id]) {
    return;
  }

  AddEventToSendToAllPlayers(Event(EventType::kStartShootingAnimation,
                                   player_id));

  auto player_in_model =
      start_model->GetPlayerByPlayerId(player_id);

  std::vector<GameObjectId> bullet_ids = AddBullets(
      start_model, player_id, player_in_model->GetX(),
      player_in_model->GetY(), player_in_model->GetRotation(),
      player_in_model->GetWeapon(), event.GetArg<QList<QVariant>>(2));

  int start_model_id = model_id;
  for (int bullet_id : bullet_ids) {
    model_id = start_model_id;
    auto prev_bullet =
        std::dynamic_pointer_cast<Bullet>(
            start_model->GetGameObjectByGameObjectId(bullet_id));

    Event event_to_send(EventType::kSendGameInfoToInterpolate,
                bullet_id,
                static_cast<int>(GameObjectType::kBullet),
                static_cast<qint64>(timestamp),
                static_cast<int>(EventType::kUpdateGameObjectData),
                bullet_id);
    event_to_send.PushBackArgs(prev_bullet->GetParams());
    auto players = start_model->GetPlayers();
    std::vector<GameObjectId> player_list;
    for (const auto& player : players) {
      if (prev_bullet->GetParentId() == player->GetId()) {
        continue;
      }
      player_list.push_back(player->GetId());
    }
    this->AddEventToSendToPlayerList(event_to_send, player_list);

    auto start_player = start_model->GetPlayerByPlayerId(player_id);
    start_player->GetWeapon()->SetLastTimeShot(timestamp);
    auto interpolation_msecs_model_before = timestamp -
        Constants::kInterpolationMSecs;
    model_id++;
    interpolation_msecs_model_before += Constants::kTimeToTick;
    while (model_id != static_cast<int>(models_cache_.size())) {
      auto cur_model = models_cache_[model_id].model;
      auto new_bullet =
          std::dynamic_pointer_cast<Bullet>(prev_bullet->Clone());
      cur_model->AttachGameObject(bullet_id, new_bullet);
      auto prev_model_id =
          GetModelIdByTimestamp(interpolation_msecs_model_before);
      if (prev_model_id >= 0) {
        auto prev_model = models_cache_[prev_model_id].model;
        this->ProcessBulletHits(
            models_cache_[model_id], new_bullet,
            prev_model->GetAllExistGameObjects());
      }
      new_bullet->OnTick(models_cache_[model_id].delta_time);
      prev_bullet = new_bullet;

      model_id++;
      interpolation_msecs_model_before += Constants::kTimeToTick;
    }
  }
}

void RoomController::ReviveConfirmedEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectIdTaken(player_id)) {
    return;
  }
  are_controls_blocked_[player_id] = false;
}

void RoomController::RequestRespawnEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  if (!model_->IsGameObjectIdTaken(player_id)) {
    return;
  }
  this->AddEventToSendToAllPlayers(Event(EventType::kPlayerRespawned,
                                         player_id));
  auto player = model_->GetPlayerByPlayerId(player_id);
  QPointF point_to_spawn = model_->GetPointToSpawn(
      player->GetRigidBodyBoundingCircleRadius(), GameObjectType::kPlayer);
  player->Revive(point_to_spawn);
  this->AddEventToSendToSinglePlayer(
      Event(EventType::kReviveLocalPlayer, point_to_spawn), player_id);
  are_controls_blocked_[player_id] = true;
}
