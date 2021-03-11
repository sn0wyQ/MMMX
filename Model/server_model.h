#ifndef MODEL_SERVER_MODEL_H_
#define MODEL_SERVER_MODEL_H_

#include <map>
#include <memory>
#include <queue>
#include <utility>

#include <QAbstractSocket>
#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>

#include "Event/event.h"
#include "Server/Room/room_controller.h"
#include "constants.h"

class ServerModel {
 public:
  struct ConnectedClient {
    explicit ConnectedClient(const std::shared_ptr<QWebSocket>& socket_ptr,
                             RoomId room_id = Constants::kNullRoomId);

    std::shared_ptr<QWebSocket> socket;
    // TODO(Everyone): make Client send nickname after connection to server
    QString nickname;
    RoomId room_id = Constants::kNullRoomId;
  };

  ServerModel() = default;
  ~ServerModel() = default;

  RoomId AddNewRoom();

  std::shared_ptr<ConnectedClient>
    GetClientByClientId(ClientId client_id) const;
  std::map<RoomId, std::shared_ptr<RoomController>>& GetRooms();
  std::shared_ptr<RoomController> GetRoomByRoomId(RoomId room_id) const;
  std::shared_ptr<RoomController> GetRoomByClientId(ClientId client_id) const;

  void AddToRoomsWithFreeSpot(RoomId room_id);

  bool IsRoomsQueueEmpty() const;
  std::shared_ptr<RoomController> GetTopRoomInQueue() const;
  void PopTopRoomInQueue();

  ClientId GetClientIdByWebSocket(QWebSocket* web_socket) const;

  ClientId GetNextUnusedClientId() const;

  void DeleteRoom(RoomId room_id);

  void SetConnectedClient(ClientId client_id,
                          std::shared_ptr<ConnectedClient> connected_client);
  void SetClientIdToWebSocket(const std::shared_ptr<QWebSocket>& web_socket,
                              ClientId client_id);
  void AddClientToRoom(RoomId room_id, ClientId client_id);

  void RemoveClient(ClientId client_id);

 private:
  std::map<ClientId, std::shared_ptr<ConnectedClient>> connected_clients_;
  std::map<QWebSocket*, ClientId> client_ids_;
  std::map<RoomId, std::shared_ptr<RoomController>> rooms_;
  std::queue<std::shared_ptr<RoomController>> rooms_queue_;
};

#endif  // MODEL_SERVER_MODEL_H_
