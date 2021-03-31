#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), room_settings_(room_settings) {
  this->StartTicking();
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
  this->TickPlayers(delta_time);
}

void RoomController::TickPlayers(int delta_time) {
  std::vector<std::shared_ptr<Player>> players = model_.GetPlayers();
  for (const auto& player : players) {
    player->OnTick(delta_time);
  }
}

void RoomController::AddClient(ClientId client_id) {
  ShareGameObjectsToClient(client_id);

  GameObjectId player_id = AddDefaultPlayer();
  player_ids_[client_id] = player_id;

  Event event(EventType::kUpdateGameObjectData,
              player_id,
              static_cast<int>(GameObjectType::kPlayer));
  auto player_ptr = model_.GetPlayerByPlayerId(player_id);
  event.PushBackArgs(player_ptr->GetParams());
  this->AddEventToSendToAllClients(event);

  this->AddEventToSendToSingleClient(
      Event(EventType::kSetPlayerIdToClient, player_id), client_id);

  qInfo().noquote().nospace() << "[ROOM ID: " << id_
          << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToSendToAllClients(Event(EventType::kStartGame));
    room_state_ = RoomState::kGameInProgress;
    qInfo().noquote().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
  player_ptr->SetFovRadius(Constants::kDefaultPlayersFov);
  this->AddEventToSendToSinglePlayer(
      Event(EventType::kUpdatePlayersFovRadius,
            player_ptr->GetFovRadius()), player_id);
}

void RoomController::RemoveClient(ClientId client_id) {
  GameObjectId player_id = ClientIdToPlayerId(client_id);
  if (player_id == Constants::kNullClientId) {
    throw std::runtime_error(
        "[ROOM ID:" + std::to_string(id_) + "] Invalid client ID");
  }
  model_.DeleteGameObject(player_id);
  this->AddEventToSendToAllClients(Event(EventType::kPlayerDisconnected,
                                         player_id));
  player_ids_.erase(client_id);
  room_state_ = (model_.GetPlayersCount()
      ? RoomState::kWaitingForClients : RoomState::kGameFinished);
  qInfo().noquote().nospace() << "[ROOM ID: " << id_
                    << "] Removed Player ID: " << player_id;
}

bool RoomController::HasFreeSpot() const {
  return model_.GetPlayersCount() < room_settings_.GetMaxClients();
}

bool RoomController::HasPlayers() const {
  return model_.GetPlayersCount() > 0;
}

bool RoomController::IsGameInProgress() const {
  return room_state_ == RoomState::kGameInProgress;
}

bool RoomController::IsWaitingForClients() const {
  return room_state_ == RoomState::kWaitingForClients;
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

void RoomController::AddBox(float x, float y, float rotation,
                            float width, float height) {
  model_.AddGameObject(GameObjectType::kGameObject,
                       {x, y, rotation, width, height,
                        static_cast<int>(RigidBodyType::kRectangle)});
}

void RoomController::AddTree(float x, float y, float radius) {
  model_.AddGameObject(GameObjectType::kGameObject,
                       {x, y, 0.f, radius * 2.f, radius * 2.f,
                        static_cast<int>(RigidBodyType::kCircle)});
}

void RoomController::ShareGameObjectsToClient(ClientId client_id) {
  for (const auto& object : model_.GetAllGameObjects()) {
    Event event(EventType::kUpdateGameObjectData, object->GetId(),
                static_cast<int>(object->GetType()));
    event.PushBackArgs(object->GetParams());
    this->AddEventToSendToSingleClient(event, client_id);
  }
}

bool RoomController::IsObjectInFov(GameObjectId sender_player_id,
                                   GameObjectId receiver_player_id) {
  auto receiver = model_.GetPlayerByPlayerId(receiver_player_id);
  auto sender = model_.GetGameObjectByGameObjectId(sender_player_id);

  return QLineF(receiver->GetPosition(), sender->GetPosition()).length() <
      receiver->GetFovRadius() + receiver->GetRadius();
}

void RoomController::UpdateReceiversByFov(
    GameObjectId sender_player_id,
    std::vector<GameObjectId>* data_receivers,
    std::vector<GameObjectId>* left_fov_event_receivers) {
  for (auto& [receiver_client_id,
    receiver_player_id] : this->player_ids_) {
    auto receiver = model_.GetPlayerByPlayerId(receiver_player_id);
    auto sender_receiver_pair = std::make_pair(sender_player_id,
                                               receiver_player_id);
    if (this->IsObjectInFov(sender_player_id, receiver_player_id)) {
      is_first_in_fov_of_second_.insert(sender_receiver_pair);
      data_receivers->push_back(receiver_player_id);
    } else if (is_first_in_fov_of_second_.find(sender_receiver_pair)
        != is_first_in_fov_of_second_.end()) {
      left_fov_event_receivers->push_back(receiver_player_id);
      is_first_in_fov_of_second_.erase(sender_receiver_pair);
    }
  }
}

GameObjectId RoomController::AddDefaultPlayer() {
  return model_.AddGameObject(
      GameObjectType::kPlayer,
      {Constants::kDefaultPlayerX, Constants::kDefaultPlayerY,
       Constants::kDefaultPlayerRotation, Constants::kDefaultPlayerRadius * 2,
       Constants::kDefaultPlayerRadius * 2, 0.f, 0.f,
       static_cast<int>(RigidBodyType::kCircle)});
}

// ------------------- GAME EVENTS -------------------

void RoomController::SendControlsEvent(const Event& event) {
  auto sender_player_id = event.GetArg<GameObjectId>(0);

  if (!model_.IsGameObjectIdTaken(sender_player_id)) {
    return;
  }

  auto sender_player_ptr = model_.GetPlayerByPlayerId(sender_player_id);

  // TODO(Everyone): add anti-cheat mechanism to check
  //  if it was possible for player to travel this distance so fast
  sender_player_ptr->SetX(event.GetArg<float>(1));
  sender_player_ptr->SetY(event.GetArg<float>(2));
  sender_player_ptr->SetVelocity(event.GetArg<QVector2D>(3));
  sender_player_ptr->SetRotation(event.GetArg<float>(4));

  std::vector<GameObjectId> data_receivers;
  std::vector<GameObjectId> left_fov_event_receivers;

  UpdateReceiversByFov(sender_player_id, &data_receivers,
                       &left_fov_event_receivers);

  if (!data_receivers.empty()) {
    Event new_event(EventType::kUpdateGameObjectData,
                      sender_player_id,
                      static_cast<int>(GameObjectType::kPlayer));
    new_event.PushBackArgs(sender_player_ptr->GetParams());
    this->AddEventToSendToPlayerList(new_event, data_receivers);
  }
  if (!left_fov_event_receivers.empty()) {
    this->AddEventToSendToPlayerList(
        Event(EventType::kGameObjectLeftFov,
              sender_player_id), left_fov_event_receivers);
  }
}

void RoomController::GameObjectLeftFovEvent(const Event& event) {
  this->AddEventToSendToAllPlayers(event);
}
