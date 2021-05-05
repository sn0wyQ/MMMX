#ifndef SERVER_SERVER_CONTROLLER_H_
#define SERVER_SERVER_CONTROLLER_H_

#include <memory>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QString>
#include <QWebSocketServer>

#include "Controller/base_controller.h"
#include "server_model.h"

namespace Constants::ServerController {

constexpr int kMinimumNumberOfPublicRoomsWithFreePlace = 3;

}  // namespace Constants::ServerController

class ServerController : public BaseController {
  Q_OBJECT

 public:
  ServerController();
  ~ServerController() override = default;

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick(int delta_time) override;

  void SendVisibleRoomsInfo(ClientId client_id);

  public Q_SLOTS:
  void OnByteArrayReceived(const QByteArray& message);
  void OnNewClient();
  void OnSocketDisconnected();

 private:
  // Helpers for Send()
  void SendToClient(int client_id, const Event& event);
  void SendToRoom(int room_id, const Event& event);

  void ProcessEventsFromRooms();
  void ProcessEventsFromRoom(const std::shared_ptr<RoomController>& room_ptr);

  void ReplyWithTimeToClient(ClientId client_id, int64_t client_time);

  QWebSocketServer web_socket_server_;
  ServerModel server_model_;

  void ClientDisconnectedEvent(const Event& event) override;
  void SendEventToClientEvent(const Event& event) override;
  void SendEventToRoomEvent(const Event& event) override;
};

#endif  // SERVER_SERVER_CONTROLLER_H_
