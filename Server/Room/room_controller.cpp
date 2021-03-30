#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), room_settings_(room_settings) {
  this->StartTicking();
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
  this->AddEventToHandle(Event(EventType::kUpdatePlayersFovRadius,
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

GameObjectId RoomController::GetNextUnusedPlayerId() const {
  GameObjectId player_id = 1;
  while (model_.IsPlayerIdTaken(player_id)) {
    player_id++;
  }
  return player_id;
}

void RoomController::AddNewPlayerEvent(const Event& event) {
  this->AddEventToSendToAllClients(event);
  this->AddEventToSendToSingleClient(Event(EventType::kSetClientsPlayerId,
                                           event.GetArg<GameObjectId>(1)),
                                     event.GetArg<ClientId>(0));
}

void RoomController::CreateAllPlayersDataEvent(const Event& event) {
  this->AddEventToSendToSingleClient(event, event.GetArg<ClientId>(0));
}

void RoomController::StartGameEvent(const Event& event) {
  this->AddEventToSendToAllClients(Event(EventType::kStartGame));
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

bool RoomController::IsPlayerInFov(GameObjectId sender_player_id,
                                   GameObjectId receiver_player_id) {
  auto receiver = model_.GetPlayerByPlayerId(receiver_player_id);
  auto sender = model_.GetPlayerByPlayerId(sender_player_id);

  return QLineF(receiver->GetPosition(), sender->GetPosition()).length() <
      receiver->GetFovRadius() + Constants::kPlayerRadius;
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
    if (this->IsPlayerInFov(sender_player_id, receiver_player_id)) {
      is_first_in_fov_of_second_.insert(sender_receiver_pair);
      data_receivers->push_back(receiver_player_id);
    } else if (is_first_in_fov_of_second_.find(sender_receiver_pair)
        != is_first_in_fov_of_second_.end()) {
      left_fov_event_receivers->push_back(receiver_player_id);
      is_first_in_fov_of_second_.erase(sender_receiver_pair);
    }
  }
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

  std::vector<GameObjectId> data_receivers;  // client_ids here
  std::vector<GameObjectId> left_fov_event_receivers;  // and here

  UpdateReceiversByFov(sender_player_id,
                       &data_receivers, &left_fov_event_receivers);

  if (!data_receivers.empty()) {
    this->AddEventToSendToPlayerList(
        Event(EventType::kUpdatePlayerData,
              sender_player_id,
              sender_player_ptr->GetX(),
              sender_player_ptr->GetY(),
              sender_player_ptr->GetVelocity(),
              sender_player_ptr->GetViewAngle()), data_receivers);
  }
  if (!left_fov_event_receivers.empty()) {
    this->AddEventToSendToPlayerList(
        Event(EventType::kPlayerLeftFov,
              sender_player_id), left_fov_event_receivers);
  }
}

void RoomController::UpdatePlayersFovRadiusEvent(const Event& event) {
  GameObjectId player_id = this->ClientIdToPlayerId(
      event.GetArg<ClientId>(0));
  model_.GetPlayerByPlayerId(player_id)->SetFovRadius(
      event.GetArg<float>(1));
  this->AddEventToSendToAllPlayers(event);
}

void RoomController::PlayerLeftFovEvent(const Event& event) {
  this->AddEventToSendToAllPlayers(event);
}
