#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), room_settings_(room_settings) {
  this->StartTicking();
}

void RoomController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  events_for_server_.push_back(event);
}

void RoomController::OnTick(int time_from_previous_tick) {
  this->TickPlayers(time_from_previous_tick);
}

void RoomController::TickPlayers(int time_from_previous_tick) {
  std::vector<std::shared_ptr<Player>> players = model_.GetPlayers();
  for (const auto& player : players) {
    player->OnTick(time_from_previous_tick);
  }
}

void RoomController::AddClient(ClientId client_id) {
  std::vector<QVariant> event_args{client_id, model_.GetPlayersCount()};
  for (const auto& player : model_.GetPlayers()) {
    event_args.emplace_back(player->GetId());
    event_args.emplace_back(player->GetX());
    event_args.emplace_back(player->GetY());
    event_args.emplace_back(player->GetViewAngle());
  }
  this->AddEventToHandle(Event(EventType::kCreateAllPlayersData,
                               event_args));

  GameObjectId player_id = this->GetNextUnusedPlayerId();
  player_ids_.emplace(std::make_pair(client_id, player_id));
  model_.AddPlayer(player_id);
  this->AddEventToHandle(Event(EventType::kAddNewPlayer,
                               client_id, player_id));
  qInfo().noquote().nospace() << "[ROOM ID: " << id_
          << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToHandle(Event(EventType::kStartGame));
    qInfo().noquote().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
  this->AddEventToHandle(Event(EventType::kUpdatePlayersFov,
                               client_id,
                               Constants::kDefaultPlayersFov));
}

void RoomController::RemoveClient(ClientId client_id) {
  GameObjectId player_id = ClientIdToPlayerId(client_id);
  if (player_id == Constants::kNullClientId) {
    throw std::runtime_error(
        "[ROOM ID:" + std::to_string(id_) + "] Invalid client ID");
  }
  model_.DeletePlayer(player_id);
  this->AddEventToSend(Event(EventType::kPlayerDisconnected, player_id));
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

void RoomController::CreateAllPlayersDataEvent(const Event& event) {
  this->AddEventToSend(event);
}

void RoomController::StartGameEvent(const Event& event) {
  this->AddEventToSend(Event(EventType::kStartGame));
  room_state_ = RoomState::kGameInProgress;
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

void RoomController::UpdateVarsEvent(const Event& event) {
  this->AddEventToSend(Event(event.GetType(),
                             PlayerIdToClientId(
                                 event.GetArg<GameObjectId>(0))));
}

void RoomController::SendEventToClientsListEvent(const Event& event) {
  this->AddEventToSend(event);
}

bool RoomController::IsPlayerInFOV(GameObjectId sender_player_id,
                                   GameObjectId receiver_player_id) {
  auto receiver = model_.GetPlayerByPlayerId(receiver_player_id);
  auto sender = model_.GetPlayerByPlayerId(sender_player_id);

  return QLineF(receiver->GetPosition(), sender->GetPosition()).length() <
      receiver->GetFovRadius() + Constants::kPlayerRadius;
}

// ------------------- GAME EVENTS -------------------

void RoomController::SendControlsEvent(const Event& event) {
  if (!model_.IsPlayerIdTaken(event.GetArg<GameObjectId>(0))) {
    return;
  }
  auto sender_player_id = event.GetArg<GameObjectId>(0);
  auto sender_player_ptr = model_.GetPlayerByPlayerId(sender_player_id);

  // TODO(Everyone): add anti-cheat mechanism to check
  //  if it was possible for player to travel this distance so fast
  sender_player_ptr->SetX(event.GetArg<float>(1));
  sender_player_ptr->SetY(event.GetArg<float>(2));
  sender_player_ptr->SetVelocity(event.GetArg<QVector2D>(3));
  sender_player_ptr->SetViewAngle(event.GetArg<float>(4));

  QList<QVariant> data_receivers;  // client_ids here
  QList<QVariant> left_fov_event_receivers;  // and here

  for (auto& [receiver_client_id,
              receiver_player_id] : this->player_ids_) {
    auto receiver = model_.GetPlayerByPlayerId(receiver_player_id);
    if (this->IsPlayerInFOV(sender_player_id, receiver_player_id)) {
      is_first_in_fov_of_second_.emplace(sender_player_id, receiver_player_id);
      data_receivers.push_back(receiver_client_id);
    } else if (is_first_in_fov_of_second_.contains(std::make_pair(sender_player_id,
                                                                  receiver_player_id))) {
      left_fov_event_receivers.push_back(receiver_client_id);
      is_first_in_fov_of_second_.erase(std::make_pair(sender_player_id,
                                                      receiver_player_id));
    }
  }

  if (!data_receivers.empty()) {
    this->AddEventToSend(Event(EventType::kSendEventToClientsList,
                               data_receivers,
                               static_cast<int>(EventType::kUpdatePlayerData),
                               sender_player_id,
                               sender_player_ptr->GetX(),
                               sender_player_ptr->GetY(),
                               sender_player_ptr->GetVelocity(),
                               sender_player_ptr->GetViewAngle()));
  }
  if (!left_fov_event_receivers.empty()) {
    this->AddEventToSend(Event(EventType::kSendEventToClientsList,
                               left_fov_event_receivers,
                               static_cast<int>(EventType::kPlayerLeftFov),
                               sender_player_id));
  }
}

void RoomController::UpdatePlayersFovEvent(const Event& event) {
  GameObjectId player_id = this->ClientIdToPlayerId(
      event.GetArg<ClientId>(0));
  model_.GetPlayerByPlayerId(player_id)->SetFovRadius(
      event.GetArg<float>(1));
  this->AddEventToSend(event);
}

void RoomController::PlayerLeftFovEvent(const Event& event) {
  this->AddEventToSend(event);
}
