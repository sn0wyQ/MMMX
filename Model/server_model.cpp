#include "server_model.h"

ServerModel::ConnectedClient::ConnectedClient(
    const std::shared_ptr<QWebSocket>& socket_ptr,
    RoomId room_id)
  : room_id(room_id) {
  socket = std::shared_ptr<QWebSocket>(socket_ptr);
}

std::shared_ptr<ServerModel::ConnectedClient>
    ServerModel::AddConnectedClient(QWebSocketServer* web_socket_server) {
  std::shared_ptr<QWebSocket> current_socket(
      web_socket_server->nextPendingConnection());

  // TODO(Everyone): some mechanism to restore client state if it had
  //  disconnected during last X seconds

  ClientId new_client_id = GetNextUnusedClientId();
  RoomId room_id;

  bool has_room_with_free_spot = false;
  while (!has_room_with_free_spot
        && !rooms_with_free_spot_.empty()) {
    if (rooms_with_free_spot_.front()->HasFreeSpot()
        && rooms_with_free_spot_.front()->IsWaitingForClients()) {
      room_id = rooms_with_free_spot_.front()->GetId();
      has_room_with_free_spot = true;
    } else {
      rooms_with_free_spot_.pop();
    }
  }

  if (!has_room_with_free_spot) {
    room_id = this->AddNewRoom();
    this->AddToRoomsWithFreeSpot(room_id);
  }

  auto new_client = std::make_shared<ConnectedClient>(current_socket, room_id);
  connected_clients_.emplace(std::make_pair(new_client_id, new_client));
  client_ids_.emplace(std::make_pair(current_socket.get(), new_client_id));
  rooms_.at(room_id)->AddClient(new_client_id);

  qInfo().nospace() << "[SERVER] New Client: ID - " << new_client_id
          << ", RoomController - " << room_id;

  return new_client;
}

template<typename... Args>
RoomId ServerModel::AddNewRoom(Args... args) {
  RoomId room_id = (rooms_.empty() ? 1 : rooms_.rbegin()->first + 1);
  auto room = new RoomController(room_id, args...);
  rooms_.emplace(room_id, std::shared_ptr<RoomController>(room));
  qInfo().nospace() << "[SERVER] Created new RoomController (ID: "
                    << room_id << ")";
  emit(CreatedNewRoom(room_id));
  return room_id;
}

std::shared_ptr<ServerModel::ConnectedClient> ServerModel::GetClientByClientId(
    ClientId client_id) const {
  return connected_clients_.at(client_id);
}

std::shared_ptr<RoomController>
    ServerModel::GetRoomByRoomId(RoomId room_id) const {
  return rooms_.at(room_id);
}

std::shared_ptr<RoomController>
    ServerModel::GetRoomByClientId(ClientId client_id) const {
  if (client_id == Constants::kNullClientId) {
    // TODO(Everyone): throw some error and maybe even try to catch it
  }
  RoomId room_id = connected_clients_.at(client_id)->room_id;
  return rooms_.at(room_id);
}

void ServerModel::AddToRoomsWithFreeSpot(RoomId room_id) {
  rooms_with_free_spot_.emplace(rooms_.at(room_id));
}

ClientId ServerModel::GetClientIdByWebSocket(QWebSocket* web_socket) const {
  auto iter = client_ids_.find(web_socket);
  if (iter == client_ids_.end()) {
    return Constants::kNullClientId;
  }
  return iter->second;
}

ClientId ServerModel::GetNextUnusedClientId() const {
  return (connected_clients_.empty() ?
            1 : connected_clients_.rbegin()->first + 1);
}
