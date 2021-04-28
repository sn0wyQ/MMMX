#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), model_(std::make_shared<RoomGameModel>()),
      room_settings_(room_settings) {
  this->StartTicking();
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
  this->AddEventToSendToSingleClient(
      event, this->PlayerIdToClientId(player_id));
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
  this->SendPlayersStatsToPlayers();
  model_->UpdatePlayerStatsHashes();
  for (const auto& player_id : this->GetAllPlayerIds()) {
    // Рассказываем НАМ о других с учетом FOV
    SendGameObjectsDataToPlayer(player_id);
  }
  model_->UpdateGameObjectHashes();
  if (models_cache_.size() > Constants::kTicksToStore) {
    models_cache_.pop_front();
  }
}

void RoomController::RecalculateModel(const ModelData& model_data) {
  this->ProcessBulletsHits(model_data);
  this->TickObjectsInModel(model_data);
  this->DeleteReadyToBeDeletedObjects(model_data);
}

void RoomController::DeleteReadyToBeDeletedObjects(
    const ModelData& model_data) {
  auto game_objects = model_data.model->GetAllGameObjects();
  for (const auto& game_object : game_objects) {
    if (game_object->IsNeedToDelete()) {
      this->AddEventToSendToAllPlayers(
          GetEventOfGameObjectLeftFov(game_object->GetId()));
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

void RoomController::ProcessBulletsHits(const ModelData& model_data) {
  auto game_objects = model_data.model->GetAllGameObjects();
  auto bullets = model_data.model->GetAllBullets();
  std::vector<GameObjectId> objects_to_delete;
  for (const auto& bullet : bullets) {
    auto object_collided =
        ObjectCollision::GetObjectBulletCollidedWith(
            bullet, game_objects, model_data.delta_time, false);
    if (object_collided != nullptr) {
      objects_to_delete.push_back(bullet->GetId());

      if (object_collided->IsEntity()) {
        auto entity = std::dynamic_pointer_cast<Entity>(object_collided);
        float cur_entity_hp = entity->GetHealthPoints();
        float hp_to_set = std::max(0.f,
                                   cur_entity_hp - bullet->GetBulletDamage());

        if (hp_to_set == 0) {
          entity->Revive();
          if (entity->GetType() == GameObjectType::kPlayer) {
            this->AddEventToSendToSinglePlayer(
                Event(EventType::kLocalPlayerDied),
                entity->GetId());
            auto killer_id = bullet->GetParentId();
            if (model_data.model->IsGameObjectIdTaken(killer_id)) {
              auto killer = model_data.model->GetPlayerByPlayerId(killer_id);
              float receive_exp = static_cast<float>(
                  std::dynamic_pointer_cast<Player>(entity)->GetLevel())
                  * Constants::kExpMultiplier;
              killer->IncreaseExperience(receive_exp);
              auto killer_stats =
                  model_data.model->GetPlayerStatsByPlayerId(killer_id);
              killer_stats->SetKills(killer_stats->GetKills() + 1);
              killer_stats->SetLevel(killer->GetLevel());
              this->AddEventToSendToSinglePlayer(
                  Event(EventType::kIncreaseLocalPlayerExperience,
                        receive_exp),
                  bullet->GetParentId());
            }
          }
        } else {
          entity->SetHealthPoints(hp_to_set);
          if (entity->GetType() == GameObjectType::kPlayer) {
            this->AddEventToSendToSinglePlayer(
                Event(EventType::kUpdateLocalPlayerHealthPoints, hp_to_set),
                entity->GetId());
          }
        }
      }
    }
  }
  for (const auto& game_object_id : objects_to_delete) {
    this->AddEventToSendToAllPlayers(
        GetEventOfGameObjectLeftFov(game_object_id));
    model_data.model->DeleteGameObject(game_object_id);
  }
}

void RoomController::AddClient(ClientId client_id) {
  GameObjectId player_id = AddPlayer();
  player_ids_[client_id] = player_id;

  Event event_add_local_player(EventType::kAddLocalPlayerGameObject, player_id);
  event_add_local_player.PushBackArgs(
      model_->GetGameObjectByGameObjectId(player_id)->GetParams());
  this->AddEventToSendToSinglePlayer(event_add_local_player, player_id);
  this->AddEventToSendToSingleClient(
      Event(EventType::kSetPlayerIdToClient, player_id), client_id);
  auto player = model_->GetPlayerByPlayerId(player_id);
  model_->AddPlayerStats(player_id,
                         QString("Player#") + QString::number(player_id),
                         player->GetLevel());
  this->ForceSendPlayersStatsToPlayer(player_id);

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
  model_->DeleteGameObject(player_id);
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

int RoomController::GetPlayersCount() const {
  return player_ids_.size();
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

Event RoomController::GetEventOfGameObjectLeftFov(
    GameObjectId game_object_id) const {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  Event event(EventType::kSendGameInfoToInterpolate,
              game_object_id,
              static_cast<int>(game_object->GetType()),
              static_cast<qint64>(GetCurrentServerTime()),
              static_cast<int>(EventType::kGameObjectLeftFov),
              game_object_id);
  return event;
}

void RoomController::SendGameObjectsDataToPlayer(GameObjectId player_id) {
  for (const auto& object : model_->GetAllGameObjects()) {
    auto sender_receiver_pair = std::make_pair(object->GetId(), player_id);
    if (object->GetId() == player_id) {
      continue;
    }

    if (object->GetType() == GameObjectType::kBullet) {
      auto bullet = std::dynamic_pointer_cast<Bullet>(object);
      if (bullet->GetParentId() == player_id) {
        continue;
      }
    }

    if (this->IsGameObjectInFov(object->GetId(), player_id)) {
      if (is_first_in_fov_of_second_.find(sender_receiver_pair)
          == is_first_in_fov_of_second_.end() ||
          model_->IsNeededToSendGameObjectData(object->GetId())) {
        this->AddEventToSendToSinglePlayer(
            GetEventOfGameObjectData(object->GetId()), player_id);
      }
      is_first_in_fov_of_second_.insert(sender_receiver_pair);
    } else if (is_first_in_fov_of_second_.find(sender_receiver_pair)
        != is_first_in_fov_of_second_.end()) {
      is_first_in_fov_of_second_.erase(sender_receiver_pair);
      this->AddEventToSendToSinglePlayer(
          GetEventOfGameObjectLeftFov(object->GetId()), player_id);
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

bool RoomController::IsGameObjectInFov(GameObjectId game_object_id,
                                       GameObjectId player_id) {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  auto player = model_->GetPlayerByPlayerId(player_id);
  if (!game_object->IsFilteredByFov()) {
    return true;
  }
  return player->GetShortestDistance(game_object) <
      player->GetFovRadius() * Constants::kFovMultiplier;
}

// Temporary -> AddPlayer(PlayerType)
GameObjectId RoomController::AddPlayer() {
  std::vector<QVariant>
      params = {Constants::kDefaultPlayerX,
                Constants::kDefaultPlayerY,
                Constants::kDefaultPlayerRotation,
                Constants::kDefaultPlayerRadius * 2,
                Constants::kDefaultPlayerRadius * 2,
                static_cast<int>(RigidBodyType::kCircle),
                0.f, 0.f, Constants::kDefaultEntityFov * 2.f,
                Constants::kDefaultMaxHealthPoints,
                Constants::kDefaultHealthRegenSpeed,
                Constants::kDefaultMaxHealthPoints};
  // Temporary
  int players_type = this->GetPlayersCount() % Constants::kDefaultMaxClients;
  switch (players_type) {
    case 0: {
      params.emplace_back(static_cast<int>(WeaponType::kAssaultRifle));
      break;
    }
    case 1: {
      params.emplace_back(static_cast<int>(WeaponType::kCrossbow));
      break;
    }
    case 2: {
      params.emplace_back(static_cast<int>(WeaponType::kMachineGun));
      break;
    }
    case 3: {
      params.emplace_back(static_cast<int>(WeaponType::kShotgun));
      break;
    }
  }
  return model_->AddGameObject(GameObjectType::kPlayer, params);
}

void RoomController::AddBox(float x, float y, float rotation,
                            float width, float height) {
  model_->AddGameObject(GameObjectType::kGameObject,
                        {x, y, rotation, width, height,
                         static_cast<int>(RigidBodyType::kRectangle)});
}

void RoomController::AddTree(float x, float y, float radius) {
  model_->AddGameObject(GameObjectType::kGameObject,
                        {x, y, 0.f, radius * 2.f, radius * 2.f,
                         static_cast<int>(RigidBodyType::kCircle)});
}

std::vector<GameObjectId> RoomController::AddBullets(GameObjectId parent_id,
                                                     float x,
                                                     float y,
                                                     float rotation,
                                                     const std::shared_ptr<
                                                         Weapon>& weapon) {
  std::vector<std::vector<QVariant>> bullets_params =
      weapon->GetBulletsParams(parent_id, x, y, rotation);
  std::vector<GameObjectId> bullet_ids(bullets_params.size());
  for (const std::vector<QVariant>& bullet_params : bullets_params) {
    bullet_ids.emplace_back(model_->AddGameObject(
        GameObjectType::kBullet,
        bullet_params));
  }
  return bullet_ids;
}

void RoomController::AddConstantObjects() {
  this->AddBox(-5.f, -15.f, 45.f, 20.f, 10.f);
  this->AddBox(12.f, -10.f, 120.f, 20.f, 10.f);
  this->AddBox(15.f, -11.f, 120.f, 20.f, 10.f);
  this->AddBox(-10.f, -10.f, 30.f, 20.f, 10.f);
  this->AddBox(25.f, 0.f, 0.f, 20.f, 10.f);
  this->AddBox(30.f, 5.f, 0.f, 20.f, 10.f);
  this->AddTree(9.f, 7.f, 2.f);
  this->AddTree(13.f, 6.f, 2.5f);
  this->AddTree(10.f, 8.5f, 3.f);
  this->AddTree(10.f, 15.f, 1.f);
  this->AddTree(30.f, 30.f, 1.f);
}

int RoomController::GetModelIdByTimestamp(int64_t timestamp) const {
  int64_t latency = GetCurrentServerTime() - timestamp;
  latency = std::max(static_cast<int64_t>(0), latency);
  int latency_in_ticks = static_cast<int>(latency / Constants::kTimeToTick);
  return static_cast<int>(models_cache_.size()) - 1 - latency_in_ticks;
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
    player_in_model->MovableObject::OnTick(models_cache_[model_id].delta_time);
    position_to_set = player_in_model->GetPosition();
    model_id++;
  }
}

void RoomController::SendPlayerShootingEvent(const Event& event) {
  auto timestamp = event.GetArg<int64_t>(0);
  auto model_id = GetModelIdByTimestamp(timestamp);
  // Проигнорим, если чел нам прислал то, что он сделал очень давно
  if (model_id < 0) {
    return;
  }
  auto current_model_data = models_cache_[model_id];
  auto player_id = event.GetArg<GameObjectId>(1);
  if (!current_model_data.model->IsGameObjectIdTaken(player_id)) {
    return;
  }
  auto player_in_model =
      current_model_data.model->GetPlayerByPlayerId(player_id);

  if (!player_in_model->GetWeapon()->IsPossibleToShoot(timestamp)) {
    return;
  }

  std::vector<GameObjectId> bullet_ids =
      AddBullets(player_id, player_in_model->GetX(), player_in_model->GetY(),
                 player_in_model->GetRotation(), player_in_model->GetWeapon());

  for (int bullet_id_from_bullets_id : bullet_ids) {
    GameObjectId bullet_id = bullet_id_from_bullets_id;

    QPointF position_to_set =
        {player_in_model->GetX(), player_in_model->GetY()};
    bool break_bullet = false;
    bool break_player = false;
    while (model_id != static_cast<int>(models_cache_.size())) {
      auto cur_model = models_cache_[model_id].model;
      if (!break_bullet && cur_model->IsGameObjectIdTaken(bullet_id)) {
        auto bullet_in_model =
            cur_model->GetGameObjectByGameObjectId(bullet_id);
        bullet_in_model->SetPosition(position_to_set);
        bullet_in_model->OnTick(models_cache_[model_id].delta_time);
        position_to_set = bullet_in_model->GetPosition();
      } else {
        break_bullet = true;
      }

      if (!break_player && cur_model->IsGameObjectIdTaken(player_id)) {
        auto player = cur_model->GetPlayerByPlayerId(player_id);
        player->GetWeapon()->SetLastTimeShot(timestamp);
      } else {
        break_player = true;
      }

      model_id++;
    }
  }
}

void RoomController::SendNicknameEvent(const Event& event) {
  auto player_id = event.GetArg<GameObjectId>(0);
  auto nickname = event.GetArg<QString>(1);
  model_->GetPlayerStatsByPlayerId(player_id)->SetNickname(nickname);
}
