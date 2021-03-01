#include "room_controller.h"

RoomController::RoomController(RoomId id, int max_clients)
    : id_(id), max_clients_(max_clients) {
  this->StartTicking();
}

void RoomController::HandleEvent(const Event& event) {
  switch (event.GetType()) {
    case EventType::kAddNewPlayer: {
      this->AddEventToSend(event);
      this->AddEventToSend(EventType::kSetClientsPlayerId,
                           event.GetArgs());
      break;
    }

    case EventType::kPressedTestButton: {
      auto senders_player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
      senders_player_ptr->ChangeTestCounter(event.GetArg(1));
      this->AddEventToSend(EventType::kChangedTestCounter,
                           senders_player_ptr->GetId(),
                           event.GetArg(1),
                           senders_player_ptr->GetTestCounterValue());
      break;
    }

    case EventType::kSharePlayersInRoomIds: {
      this->AddEventToSend(event);
      break;
    }

    case EventType::kStartGame: {
      this->AddEventToSend(EventType::kStartGame);
      room_state_ = RoomState::kInProgress;
      break;
    }

    default:
      break;
  }
}

void RoomController::Send() {
  while (this->HasEventsToSend()) {
    Event current_event = this->GetNextEventToSend();

    std::vector<ClientId> receivers;
    switch (current_event.GetType()) {
      case EventType::kSetClientsPlayerId:
        receivers.push_back(current_event.GetArg(0));
        break;

      case EventType::kSharePlayersInRoomIds:
        receivers.push_back(current_event.GetArg(0));
        break;

      default:
        break;
    }

    emit(SendEventToServer(current_event, receivers));

    qInfo().nospace() << "[ROOM ID: " << id_
                      << "] Sent " << current_event << " to Server";
  }
}

void RoomController::AddClient(ClientId client_id) {
  std::vector<int> event_args{client_id, model_.GetPlayersCount()};
  std::vector<int> all_player_ids = GetAllPlayerIds();
  event_args.insert(event_args.end(),
                    all_player_ids.begin(),
                    all_player_ids.end());
  this->AddEventToHandle(EventType::kSharePlayersInRoomIds, event_args);

  GameObjectId player_id = 1;
  while (model_.IsPlayerIdTaken(player_id)) {
    player_id++;
  }
  player_ids_.emplace(std::make_pair(client_id, player_id));
  model_.AddPlayer(player_id);
  this->AddEventToHandle(EventType::kAddNewPlayer, client_id, player_id);
  qInfo().nospace() << "[ROOM ID: " << id_
          << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToHandle(EventType::kStartGame);
    qInfo().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
}

void RoomController::RemoveClient(ClientId client_id) {
  player_ids_.erase(client_id);
}

bool RoomController::HasFreeSpot() const {
  return model_.GetPlayersCount() < max_clients_;
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
    result[index++] = client_id;
  }
  return result;
}

std::vector<GameObjectId> RoomController::GetAllPlayerIds() const {
  std::vector<GameObjectId> result(player_ids_.size());
  int index = 0;
  for (auto [client_id, player_id] : player_ids_) {
    result[index++] = player_id;
  }
  return result;
}

RoomId RoomController::GetId() const {
  return id_;
}

GameObjectId RoomController::ClientIdToPlayerId(ClientId client_id) {
  auto iter = player_ids_.find(client_id);
  if (iter == player_ids_.end()) {
    return Constants::kNullGameObjectId;
  }
  return iter->second;
}
