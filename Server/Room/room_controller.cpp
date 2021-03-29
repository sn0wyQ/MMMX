#include "room_controller.h"

RoomController::RoomController(RoomId id, RoomSettings room_settings)
    : id_(id), room_settings_(room_settings) {
  this->StartTicking();
  this->AddBox(-5.f, -15.f, 45.f);
  this->AddBox(12.f, -10.f, 120.f);
  this->AddBox(15.f, -11.f, 120.f);
  this->AddBox(-10.f, -10.f, 30.f);
  this->AddBox(25.f, 0.f, 0.f);
  this->AddBox(30.f, 5.f, 0.f);
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
  std::vector<QVariant> event_args{client_id, model_.GetPlayersCount()};
  for (const auto& player : model_.GetPlayers()) {
    event_args.emplace_back(player->GetId());
    event_args.emplace_back(player->GetX());
    event_args.emplace_back(player->GetY());
    event_args.emplace_back(player->GetRotation());
  }
  this->AddEventToHandle(Event(EventType::kCreateAllPlayersData,
                               event_args));

  GameObjectId player_id = model_.AddPlayer();
  player_ids_[client_id] = player_id;
  this->AddEventToHandle(Event(EventType::kAddNewPlayer,
                               client_id, player_id));
  qInfo().noquote().nospace() << "[ROOM ID: " << id_
          << "] Connected client (ID: " << client_id << ")";
  if (!this->HasFreeSpot()) {
    this->AddEventToHandle(Event(EventType::kStartGame));
    qInfo().noquote().nospace() << "[ROOM ID: " << id_ << "] Started Game";
  }
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

void RoomController::AddNewPlayerEvent(const Event& event) {
  this->AddEventToSendToAllClients(event);
  auto player_id = event.GetArg<GameObjectId>(1);
  this->AddEventToSendToSingleClient(Event(EventType::kSetClientsPlayerId,
                                           player_id),
                                     event.GetArg<ClientId>(0));
  ShareObjectsOnMap(player_id);
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

void RoomController::AddBox(float x, float y, float rotation) {
  float width = 20;
  float height = 10;
  model_.AddBox(x, y, rotation, width, height);
}

void RoomController::AddTree(float x, float y, float radius) {
  model_.AddTree(x, y, radius);
}

void RoomController::ShareObjectsOnMap(GameObjectId player_id) {
  for (const auto& box : model_.GetBoxes()) {
    this->AddEventToSendToSinglePlayer(
        Event(EventType::kGameObjectAppeared, box->GetId(),
              static_cast<int>(GameObjectTypeForEvents::kBox),
              box->GetX(), box->GetY(), box->GetRotation(),
              box->GetWidth(), box->GetHeight()), player_id);
  }
  for (const auto& tree : model_.GetTrees()) {
    this->AddEventToSendToSinglePlayer(
        Event(EventType::kGameObjectAppeared, tree->GetId(),
              static_cast<int>(GameObjectTypeForEvents::kTree),
              tree->GetX(), tree->GetY(), tree->GetRadius()), player_id);
  }
}

// ------------------- GAME EVENTS -------------------

void RoomController::SendControlsEvent(const Event& event) {
  if (!model_.IsGameObjectIdTaken(
      event.GetArg<GameObjectId>(0))) {
    return;
  }
  auto senders_player_ptr =
      model_.GetPlayerByPlayerId(event.GetArg<GameObjectId>(0));

  // TODO(Everyone): add anti-cheat mechanism to check
  //  if it was possible for player to travel this distance so fast
  senders_player_ptr->SetX(event.GetArg<float>(1));
  senders_player_ptr->SetY(event.GetArg<float>(2));
  senders_player_ptr->SetVelocity(event.GetArg<QVector2D>(3));
  senders_player_ptr->SetRotation(event.GetArg<float>(4));

  // TODO(Matvey): add something here to check if player is in FOV
  this->AddEventToSendToAllPlayers(Event(EventType::kUpdatePlayerData,
                                         event.GetArg<GameObjectId>(0),
                                         senders_player_ptr->GetX(),
                                         senders_player_ptr->GetY(),
                                         senders_player_ptr->GetVelocity(),
                                         senders_player_ptr->GetRotation()));
}
