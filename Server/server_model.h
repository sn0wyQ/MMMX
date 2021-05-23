#ifndef MODEL_SERVER_MODEL_H_
#define MODEL_SERVER_MODEL_H_

#include <map>
#include <memory>
#include <queue>
#include <utility>

#include <QAbstractSocket>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWebSocket>
#include <QWebSocketServer>

#include "Event/event.h"
#include "Server/Room/room_controller.h"
#include "Constants/constants.h"

class ServerModel {
 public:
  struct ConnectedClient {
    ConnectedClient(const std::shared_ptr<QWebSocket>& socket_ptr,
                    RoomId room_id = Constants::kNullRoomId);

    std::shared_ptr<QWebSocket> socket;
    // TODO(Everyone): make Client send nickname after connection to server
    QString nickname;
    RoomId room_id = Constants::kNullRoomId;
  };

  RoomId AddNewRoom(RoomSettings room_settings = RoomSettings());

  std::shared_ptr<ConnectedClient>
    GetClientByClientId(ClientId client_id) const;
  std::map<RoomId, std::shared_ptr<RoomController>>& GetRooms();
  std::shared_ptr<RoomController> GetRoomByRoomId(RoomId room_id) const;
  std::shared_ptr<RoomController> GetRoomByClientId(ClientId client_id) const;
  RoomId GetRoomIdByClientId(ClientId client_id) const;

  ClientId GetClientIdByWebSocket(QWebSocket* web_socket) const;

  ClientId GetNextUnusedClientId() const;

  QList<QVariant> GetVisibleRoomsInfo(ClientId client_id) const;

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
};

#endif  // MODEL_SERVER_MODEL_H_
