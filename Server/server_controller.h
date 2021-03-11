#ifndef SERVER_SERVER_CONTROLLER_H_
#define SERVER_SERVER_CONTROLLER_H_

#include <memory>
#include <utility>
#include <vector>

#include <QByteArray>

#include "Controller/base_controller.h"
#include "Model/server_model.h"

class ServerController : public BaseController {
  Q_OBJECT

 public:
  ServerController();
  ~ServerController() override;

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick() override;

  void NewRoomCreated(RoomId room_id);

  public Q_SLOTS:
  void OnByteArrayReceived(const QByteArray& message);
  void OnEventFromRoomReceived(const Event& event,
                               std::vector<ClientId> receivers);
  void OnNewClient();
  void OnSocketDisconnected();

 private:
  // Helpers for Send()
  void SendToClient(int client_id, const Event& event);
  void SendToRoom(int room_id, const Event& event);

  QWebSocketServer web_socket_server_;
  ServerModel model_;

  void ClientDisconnectedEvent(const Event& event) override;
  void SetClientsPlayerIdEvent(const Event& event) override;
  void SendEventToClientEvent(const Event& event) override;
  void SendEventToRoomEvent(const Event& event) override;
};

#endif  // SERVER_SERVER_CONTROLLER_H_
