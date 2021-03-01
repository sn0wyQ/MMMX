#ifndef SERVER_SERVER_CONTROLLER_H_
#define SERVER_SERVER_CONTROLLER_H_

#include <memory>
#include <utility>

#include <QByteArray>

#include "Controller/base_controller.h"
#include "Model/server_model.h"

class ServerController : public BaseController {

 public:
  ServerController();
  ~ServerController() override;

  void HandleEvent(const Event& event) override;
  void Send() override;

 public Q_SLOTS:
  void OnByteArrayReceived(const QByteArray& message);
  void OnEventFromRoomReceived(const Event& event,
                               std::vector<ClientId> receivers);
  void OnNewClient();
  void OnNewRoom(RoomId room_id);
  void OnSocketDisconnected();

 private:
  // Helpers for Send()
  void SendToClient(int client_id, const Event& event);
  void SendToRoom(int room_id, const Event& event);

  QWebSocketServer web_socket_server_;
  ServerModel model_;
};

#endif  // SERVER_SERVER_CONTROLLER_H_
