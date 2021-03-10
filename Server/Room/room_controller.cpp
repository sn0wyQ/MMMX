#include "room_controller.h"

RoomController::RoomController(RoomId id, int max_clients)
    : id_(id), room_settings_(max_clients) {
  this->StartTicking();
}

void RoomController::SendEvent(const Event& event) {
  std::vector<ClientId> receivers;
  switch (event.GetType()) {
    case EventType::kSetClientsPlayerId:
      receivers.push_back(event.GetArg(0));
      break;

    case EventType::kSharePlayersInRoomIds:
      receivers.push_back(event.GetArg(0));
      break;

    default:
      break;
  }

  emit(SendEventToServer(event, receivers));

  qInfo().nospace() << "[ROOM ID: " << id_
                    << "] Sent " << event << " to Server";
}

void RoomController::OnTick() {}

void RoomController::AddClient(ClientId client_id) {
  std::vector<int> event_args{client_id, model_.GetPlayersCount()};
  std::vector<int> all_player_ids = GetAllPlayerIds();
  event_args.insert(event_args.end(),
                    all_player_ids.begin(),
                    all_player_ids.end());
  this->AddEventToHandle(Event(EventType::kSharePlayersInRoomIds,
                               event_args));

  GameObjectId player_id = this->GetNextUnusedPlayerId();
  player_ids_.emplace(std::make_pair(client_id, player_id));
  model_.AddPlayer(player_id);
  this->AddEventToHandle(Event(EventType::kAddNewPlayer,
                               client_id, player_id));
  qInfo().nospace() << "[ROOM ID: " << id_
          << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToHandle(Event(EventType::kStartGame));
    qInfo().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
}

void RoomController::RemoveClient(ClientId client_id) {
  GameObjectId player_id = ClientIdToPlayerId(client_id);
  model_.DeletePlayer(player_id);
  this->AddEventToSend(Event(EventType::kPlayerDisconnected, player_id));
  player_ids_.erase(client_id);
  qInfo().nospace() << "[ROOM ID: " << id_
                    << "] Removed Player ID: " << player_id;
}

bool RoomController::HasFreeSpot() const {
  return model_.GetPlayersCount() < room_settings_.GetMaxClients();
}

bool RoomController::HasPlayers() const {
  return model_.GetPlayersCount() > 0;
}

bool RoomController::IsGameInProgress() const {
  return room_state_ == RoomState::kInProgress;
}

bool RoomController::IsWaitingForClients() const {
  return room_state_ == RoomState::kWaitingForClients;
}

std::vector<ClientId> RoomController::GetAllClientsIds() const {
  std::vector<ClientId> result(player_ids_.size());
  int index = 0;
  for (auto [client_id, player_id] : player_ids_) {
    result[index] = client_id;
    index++;
  }
  return result;
}

std::vector<GameObjectId> RoomController::GetAllPlayerIds() const {
  std::vector<GameObjectId> result(player_ids_.size());
  int index = 0;
  for (auto [client_id, player_id] : player_ids_) {
    result[index] = player_id;
    ++index;
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

GameObjectId RoomController::GetNextUnusedPlayerId() const {
  GameObjectId player_id = 1;
  while (model_.IsPlayerIdTaken(player_id)) {
    player_id++;
  }
  return player_id;
}

void RoomController::AddNewPlayerEvent(const Event& event) {
  this->AddEventToSend(event);
  this->AddEventToSend(Event(EventType::kSetClientsPlayerId,
                             event.GetArgs()));
}

void RoomController::PressedTestButtonEvent(const Event& event) {
  auto senders_player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
  senders_player_ptr->ChangeTestCounter(event.GetArg(1));
  this->AddEventToSend(Event(EventType::kChangedTestCounter,
                             senders_player_ptr->GetId(),
                             event.GetArg(1),
                             senders_player_ptr->GetTestCounterValue()));
}

void RoomController::SharePlayersInRoomIdsEvent(const Event& event) {
  this->AddEventToSend(event);
}

void RoomController::StartGameEvent(const Event& event) {
  this->AddEventToSend(Event(EventType::kStartGame));
  room_state_ = RoomState::kInProgress;
}
