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

#include "Event/event_type.h"
#include "Server/Room/room_controller.h"
#include "constants.h"

class ServerModel : public QObject {
  Q_OBJECT

 private:
  struct ConnectedClient {
    explicit ConnectedClient(const std::shared_ptr<QWebSocket>& socket_ptr,
                             RoomId room_id = Constants::kNullRoomId);

    std::shared_ptr<QWebSocket> socket;
    // TODO(Everyone): make Client send nickname after connection to server
    PlayerNickname nickname;
    RoomId room_id = Constants::kNullRoomId;
  };

 public:
  ServerModel() = default;
  ~ServerModel() override = default;

  std::shared_ptr<ConnectedClient>
    AddConnectedClient(QWebSocketServer* web_socket_server);
  template<typename... Args>
  RoomId AddNewRoom(Args... args);

  std::shared_ptr<ConnectedClient>
    GetClientByClientId(ClientId client_id) const;
  std::shared_ptr<RoomController> GetRoomByRoomId(RoomId room_id) const;
  std::shared_ptr<RoomController> GetRoomByClientId(ClientId client_id) const;

  void AddToRoomsWithFreeSpot(RoomId room_id);

  ClientId GetClientIdByWebSocket(QWebSocket* web_socket) const;

  ClientId GetNextUnusedClientId() const;

  Q_SIGNALS:
  void CreatedNewRoom(RoomId room_id);

 private:
  std::map<ClientId, std::shared_ptr<ConnectedClient>> connected_clients_;
  std::map<QWebSocket*, ClientId> client_ids_;
  std::map<RoomId, std::shared_ptr<RoomController>> rooms_;
  std::queue<std::shared_ptr<RoomController>> rooms_with_free_spot_;
};

#endif  // MODEL_SERVER_MODEL_H_
