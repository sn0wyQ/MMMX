#include "server_model.h"

ServerModel::ConnectedClient::ConnectedClient(
    const std::shared_ptr<QWebSocket>& socket_ptr,
    RoomId room_id)
  : room_id(room_id) {
  socket = std::shared_ptr<QWebSocket>(socket_ptr);
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
  rooms_queue_.emplace(rooms_.at(room_id));
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

bool ServerModel::IsRoomsQueueEmpty() const {
  return rooms_queue_.empty();
}

std::shared_ptr<RoomController> ServerModel::GetTopRoomInQueue() const {
  return rooms_queue_.front();
}

void ServerModel::PopTopRoomInQueue() {
  rooms_queue_.pop();
}

void ServerModel::SetConnectedClient(
    ClientId client_id, std::shared_ptr<ConnectedClient> connected_client) {
  connected_clients_.emplace(std::make_pair(client_id,
                                            std::move(connected_client)));
}

void ServerModel::SetClientIdToWebSocket(
    const std::shared_ptr<QWebSocket>& web_socket, ClientId client_id) {
  client_ids_.emplace(std::make_pair(web_socket.get(), client_id));
}

void ServerModel::AddClientToRoom(
    RoomId room_id, ClientId client_id) {
  rooms_.at(room_id)->AddClient(client_id);
}
