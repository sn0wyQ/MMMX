#include "server_controller.h"

ServerController::ServerController()
  : web_socket_server_(Constants::kServerName,
                       QWebSocketServer::NonSecureMode) {
  if (web_socket_server_.listen(QHostAddress::Any, Constants::kServerPort)) {
    qInfo() << "Server is running on " << Constants::kServerUrl.host() << ":"
    << web_socket_server_.serverPort();
    connect(&web_socket_server_,
            &QWebSocketServer::newConnection,
            this,
            &ServerController::OnNewClient);
  }

  connect(&model_,
          &ServerModel::CreatedNewRoom,
          this,
          &ServerController::OnNewRoom);

  this->StartTicking();
}

ServerController::~ServerController() {
  web_socket_server_.close();
}

void ServerController::SendEvent(const Event& event) {
  switch (event.GetType()) {
    case EventType::kSendEventToClient: {
      this->SendToClient(
          event.GetArg(0),
          Event(EventType(event.GetArg(1)),
                event.GetArgsSubVector(2)));
      break;
    }

    case EventType::kSendEventToRoom: {
      this->SendToRoom(
          event.GetArg(0),
          Event(EventType(event.GetArg(1)),
                event.GetArgsSubVector(2)));
      break;
    }

    case EventType::kSetClientsPlayerId: {
      this->SendToClient(event.GetArg(0), event);
      break;
    }

    // We only want to send Events either to Client or Room
    default:
      break;
  }
}

void ServerController::OnByteArrayReceived(const QByteArray& message) {
  auto client_socket_ptr = qobject_cast<QWebSocket*>(sender());
  auto client_id = model_.GetClientIdByWebSocket(client_socket_ptr);
  Event event(message);

  std::vector<int> args {model_.GetRoomByClientId(client_id)->GetId(),
                        static_cast<int>(event.GetType())};
  std::vector<int> old_args = event.GetArgs();
  args.insert(args.end(), old_args.begin(), old_args.end());

  this->AddEventToHandle(Event(EventType::kSendEventToRoom, args));

  qInfo() << "[SERVER] Received" << event
           << "from Client ID:" << client_id;
}

void ServerController::OnEventFromRoomReceived(
    const Event& event,
    std::vector<ClientId> receivers) {
  auto room_ptr = qobject_cast<RoomController*>(sender());
  if (receivers.empty()) {
    receivers = room_ptr->GetAllClientsIds();
  }

  std::vector<int> args = {Constants::kNullClientId,
                           static_cast<int>(event.GetType())};
  std::vector<int> old_args = event.GetArgs();
  args.insert(args.end(), old_args.begin(), old_args.end());

  for (auto client_id : receivers) {
    args.at(0) = client_id;
    this->AddEventToHandle(Event(EventType::kSendEventToClient, args));
  }

  qInfo() << "[SERVER] Received" << event
          << "from Room ID:" << room_ptr->GetId();
}

void ServerController::OnNewClient() {
  std::shared_ptr<ServerModel::ConnectedClient>
      new_client(model_.AddConnectedClient(&web_socket_server_));

  connect(new_client->socket.get(),
          &QWebSocket::binaryMessageReceived,
          this,
          &ServerController::OnByteArrayReceived);

  connect(new_client->socket.get(),
          &QWebSocket::disconnected,
          this,
          &ServerController::OnSocketDisconnected);
}

void ServerController::OnNewRoom(RoomId room_id) {
  auto room_ptr = model_.GetRoomByRoomId(room_id);
  connect(room_ptr.get(),
          &RoomController::SendEventToServer,
          this,
          &ServerController::OnEventFromRoomReceived);
}

void ServerController::OnSocketDisconnected() {
  auto web_socket = qobject_cast<QWebSocket*>(sender());
  qInfo() << "[SERVER] Socket disconnected:" << web_socket;
  if (web_socket) {
    ClientId client_id = model_.GetClientIdByWebSocket(web_socket);
    this->AddEventToHandle(Event(EventType::kClientDisconnected,
                                 client_id));
  }
}

void ServerController::SendToClient(int client_id,
                                    const Event& event) {
  auto client_ptr = model_.GetClientByClientId(client_id);
  client_ptr->socket->sendBinaryMessage(event.ToByteArray());
  qInfo() << "[SERVER] Sent" << event << "to Client ID:" << client_id;
}

void ServerController::SendToRoom(int room_id,
                                  const Event& event) {
  model_.GetRoomByRoomId(room_id)->AddEventToHandle(event);
}

void ServerController::ClientDisconnectedEvent(const Event& event) {
  ClientId client_id = event.GetArg(0);
  auto room_ptr = model_.GetRoomByClientId(client_id);
  room_ptr->RemoveClient(client_id);
}
