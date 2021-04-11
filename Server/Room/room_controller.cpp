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
  models_cache_.push_back({
    std::make_shared<RoomGameModel>(*model_), delta_time});
  this->RecalculateModel(models_cache_.back());
  model_ = models_cache_.back().model;
  for (const auto& player_id : this->GetAllPlayerIds()) {
    // Рассказываем НАМ о других с учетом FOV
    SendGameObjectsDataToPlayer(player_id);
  }
  for (const auto& game_object : model_->GetAllGameObjects()) {
    // Рассказываем ДРУГИМ о нас с учетом FOV
    SendGameObjectDataToPlayers(game_object->GetId());
  }
  if (models_cache_.size() > Constants::kTicksToStore) {
    models_cache_.pop_front();
  }
}

void RoomController::RecalculateModel(const ModelData& model_data) {
  this->TickPlayersInModel(model_data);
}

void RoomController::TickPlayersInModel(const ModelData& model_data) {
  std::vector<std::shared_ptr<Player>> players = model_data.model->GetPlayers();
  for (const auto& player : players) {
    player->OnTick(model_data.delta_time);
  }
}

void RoomController::AddClient(ClientId client_id) {
  GameObjectId player_id = AddDefaultPlayer();
  player_ids_[client_id] = player_id;

  Event event(EventType::kAddLocalPlayerGameObject, player_id);
  event.PushBackArgs(
      model_->GetGameObjectByGameObjectId(player_id)->GetParams());
  this->AddEventToSendToSinglePlayer(event, player_id);
  this->AddEventToSendToSingleClient(
      Event(EventType::kSetPlayerIdToClient, player_id), client_id);

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
  for (auto [client_id, player_id] : player_ids_) {
    result.push_back(client_id);
  }
  return result;
}

std::vector<GameObjectId> RoomController::GetAllPlayerIds() const {
  std::vector<GameObjectId> result;
  for (auto [client_id, player_id] : player_ids_) {
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
  for (auto [client_id, player_id_in] : player_ids_) {
    if (player_id == player_id_in) {
      return client_id;
    }
  }
  throw std::runtime_error("[ROOM] No such player");
}

std::vector<Event> RoomController::ClaimEventsForServer() {
  return std::move(events_for_server_);
}

Event RoomController::GetEventOfGameObjectData(GameObjectId game_object_id)
const {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  Event event(EventType::kUpdateGameObjectData,
              QDateTime::currentMSecsSinceEpoch(),
              game_object_id,
              static_cast<int>(game_object->GetType()));
  event.PushBackArgs(game_object->GetParams());
  return event;
}

void RoomController::SendGameObjectsDataToPlayer(GameObjectId player_id) {
  for (const auto& object : model_->GetAllGameObjects()) {
    auto sender_receiver_pair = std::make_pair(object->GetId(), player_id);
    if (this->IsGameObjectInFov(object->GetId(), player_id)) {
      is_first_in_fov_of_second_.insert(sender_receiver_pair);
      this->AddEventToSendToSinglePlayer(
          GetEventOfGameObjectData(object->GetId()), player_id);
    } else if (is_first_in_fov_of_second_.find(sender_receiver_pair)
        != is_first_in_fov_of_second_.end()) {
      is_first_in_fov_of_second_.erase(sender_receiver_pair);
      this->AddEventToSendToSinglePlayer(
          Event(EventType::kGameObjectLeftFov, object->GetId()), player_id);
    }
  }
}

void RoomController::SendGameObjectDataToPlayers(
    GameObjectId game_object_id) {
  std::vector<GameObjectId> data_receivers;
  std::vector<GameObjectId> left_fov_event_receivers;

  UpdateReceiversByFov(game_object_id, &data_receivers,
                       &left_fov_event_receivers);

  if (!data_receivers.empty()) {
    this->AddEventToSendToPlayerList(
        GetEventOfGameObjectData(game_object_id), data_receivers);
  }
  if (!left_fov_event_receivers.empty()) {
    this->AddEventToSendToPlayerList(
        Event(EventType::kGameObjectLeftFov,
              game_object_id), left_fov_event_receivers);
  }
}

bool RoomController::IsGameObjectInFov(GameObjectId game_object_id,
                                       GameObjectId player_id) {
  auto game_object = model_->GetGameObjectByGameObjectId(game_object_id);
  auto player = model_->GetPlayerByPlayerId(player_id);
  if (!game_object->IsFilteredByFov()) {
    return true;
  }
  return player->GetShortestDistance(game_object) < player->GetFovRadius();
}

void RoomController::UpdateReceiversByFov(
    GameObjectId game_object_id,
    std::vector<GameObjectId>* data_receivers,
    std::vector<GameObjectId>* left_fov_event_receivers) {
  for (auto& [receiver_client_id,
    receiver_player_id] : this->player_ids_) {
    auto receiver = model_->GetPlayerByPlayerId(receiver_player_id);
    auto sender_receiver_pair = std::make_pair(game_object_id,
                                               receiver_player_id);
    if (this->IsGameObjectInFov(game_object_id, receiver_player_id)) {
      is_first_in_fov_of_second_.insert(sender_receiver_pair);
      data_receivers->push_back(receiver_player_id);
    } else if (is_first_in_fov_of_second_.find(sender_receiver_pair)
        != is_first_in_fov_of_second_.end()) {
      is_first_in_fov_of_second_.erase(sender_receiver_pair);
      left_fov_event_receivers->push_back(receiver_player_id);
    }
  }
}

GameObjectId RoomController::AddDefaultPlayer() {
  float new_fov = Constants::kDefaultEntityFov * (GetPlayersCount() + 1);
  float new_radius = Constants::kDefaultPlayerRadius;
  return model_->AddGameObject(
      GameObjectType::kPlayer,
      {Constants::kDefaultPlayerX, Constants::kDefaultPlayerY,
       Constants::kDefaultPlayerRotation, new_radius * 2,
       new_radius * 2,
       static_cast<int>(RigidBodyType::kCircle), 0.f, 0.f,
       new_fov});
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

// ------------------- GAME EVENTS -------------------

void RoomController::SendControlsEvent(const Event& event) {
  auto timestamp = event.GetArg<int64_t>(0);
  int64_t latency_in_msecs = QDateTime::currentMSecsSinceEpoch() - timestamp;
  int64_t latency = latency_in_msecs / Constants::kTimeToTick;
  qInfo() << latency << models_cache_.size();
  if (latency > models_cache_.size()) {
    // kick player
    throw std::runtime_error("Too slow connection Mr. Vlad Kozulin "
                             "from Krakow");
  }
  int id_of_model = static_cast<int64_t>(models_cache_.size()) - 1 - latency;
  auto current_model = models_cache_[id_of_model];

  auto player_id = event.GetArg<GameObjectId>(1);

  if (!current_model.model->IsGameObjectIdTaken(player_id)) {
    return;
  }

  auto player = current_model.model->GetPlayerByPlayerId(player_id);

  // TODO(Everyone): add anti-cheat mechanism to check
  //  if it was possible for player to travel this distance so fast
  player->SetX(event.GetArg<float>(2));
  player->SetY(event.GetArg<float>(3));
  player->SetVelocity(event.GetArg<QVector2D>(4));
  player->SetRotation(event.GetArg<float>(5));
  while (id_of_model != models_cache_.size()) {
    auto player_in_model
      = models_cache_[id_of_model].model->GetPlayerByPlayerId(player_id);
    player_in_model->OnTick(models_cache_[id_of_model].delta_time);
    id_of_model++;
  }
}
