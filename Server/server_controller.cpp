#include "server_controller.h"

ServerController::ServerController()
  : web_socket_server_(Constants::kServerName,
                       QWebSocketServer::NonSecureMode) {
  if (web_socket_server_.listen(QHostAddress::Any,  Constants::kServerPort)) {
    qInfo().noquote().nospace() << "Server is running on "
      << Constants::kServerUrl.host() << ":"
      << web_socket_server_.serverPort();
    connect(&web_socket_server_,
            &QWebSocketServer::newConnection,
            this,
            &ServerController::OnNewClient);
  }

  this->StartTicking();
}

void ServerController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  switch (event.GetType()) {
    case EventType::kSendEventToClient: {
      this->SendToClient(
          event.GetArg<ClientId>(0),
          Event(EventType(event.GetArg<int>(1)),
                event.GetArgsSubVector(2)));
      break;
    }

    case EventType::kSendEventToRoom: {
      this->SendToRoom(
          event.GetArg<RoomId>(0),
          Event(EventType(event.GetArg<int>(1)),
                event.GetArgsSubVector(2)));
      break;
    }

    case EventType::kSetClientsPlayerId:
    case EventType::kUpdateServerVar: {
      this->SendToClient(event.GetArg<ClientId>(0), event);
      break;
    }

    // If we're here something most likely went wrong
    default:
        qWarning() << "[SERVER] Impossible to send event" << event;
      break;
  }
}

void ServerController::OnTick(int time_from_previous_tick) {
  std::vector<RoomId> rooms_to_delete;
  for (auto& room_iter : server_model_.GetRooms()) {
    if (!room_iter.second->HasPlayers()
        && !room_iter.second->IsWaitingForClients()) {
      rooms_to_delete.push_back(room_iter.second->GetId());
    }
  }
  for (const auto& room_id : rooms_to_delete) {
    server_model_.DeleteRoom(room_id);
  }
  ProcessEventsFromRooms();
}

void ServerController::ProcessEventsFromRooms() {
  auto rooms = server_model_.GetRooms();
  for (const auto& room : rooms) {
    ProcessEventsFromRoom(room.second);
  }
}

void ServerController::ProcessEventsFromRoom(
    const std::shared_ptr<RoomController>& room_ptr) {
  std::vector<Event> events_from_room = room_ptr->ClaimEventsForServer();
  for (const auto& event : events_from_room) {
    std::vector<ClientId> receivers;
    switch (event.GetType()) {
      case EventType::kSetClientsPlayerId:
      case EventType::kCreateAllPlayersData:
      case EventType::kUpdateServerVar:
        receivers.push_back(event.GetArg<ClientId>(0));
        break;

      case EventType::kGameObjectAppeared:
        receivers.push_back(
            room_ptr->PlayerIdToClientId(event.GetArg<GameObjectId>(0)));
      default:
        break;
    }
    if (receivers.empty()) {
      receivers = room_ptr->GetAllClientsIds();
    }
    std::vector<QVariant> args = {Constants::kNullClientId,
                                  static_cast<int>(event.GetType())};
    std::vector<QVariant> old_args = event.GetArgs();
    args.insert(args.end(), old_args.begin(), old_args.end());
    for (auto client_id : receivers) {
      args.at(0) = client_id;
      this->AddEventToHandle(Event(EventType::kSendEventToClient, args));
    }
  }
}

void ServerController::OnByteArrayReceived(const QByteArray& message) {
  auto client_socket_ptr = qobject_cast<QWebSocket*>(sender());
  auto client_id = server_model_.GetClientIdByWebSocket(client_socket_ptr);
  Event event(message);

  std::vector<QVariant>
      args{server_model_.GetRoomByClientId(client_id)->GetId(),
           static_cast<int>(event.GetType())};
  std::vector<QVariant> old_args = event.GetArgs();
  args.insert(args.end(), old_args.begin(), old_args.end());

  this->AddEventToHandle(Event(EventType::kSendEventToRoom, args));

  qDebug().noquote() << "[SERVER] Received" << event
                     << "from Client ID:" << client_id;
}

void ServerController::OnNewClient() {
  std::shared_ptr<QWebSocket> current_socket(
      web_socket_server_.nextPendingConnection());

  // TODO(Everyone): some mechanism to restore client state if it had
  //  disconnected during last X seconds

  ClientId new_client_id = server_model_.GetNextUnusedClientId();
  RoomId room_id;
  bool is_correct_room_found = false;

  for (auto& [current_room_id, room_ptr] : server_model_.GetRooms()) {
    if (room_ptr->HasFreeSpot() && room_ptr->IsWaitingForClients()) {
      room_id = current_room_id;
      is_correct_room_found = true;
      break;
    }
  }
  if (!is_correct_room_found) {
    room_id = server_model_.AddNewRoom();
  }

  auto new_client =
      std::make_shared<ServerModel::ConnectedClient>(current_socket, room_id);

  server_model_.SetConnectedClient(new_client_id, new_client);
  server_model_.SetClientIdToWebSocket(current_socket, new_client_id);
  server_model_.AddClientToRoom(room_id, new_client_id);

  qInfo().noquote().nospace() << "[SERVER] New Client: ID - " << new_client_id
                    << ", RoomController - " << room_id;

  connect(new_client->socket.get(),
          &QWebSocket::binaryMessageReceived,
          this,
          &ServerController::OnByteArrayReceived);

  connect(new_client->socket.get(),
          &QWebSocket::disconnected,
          this,
          &ServerController::OnSocketDisconnected);
}

void ServerController::OnSocketDisconnected() {
  auto web_socket = qobject_cast<QWebSocket*>(sender());
  qInfo().noquote() << "[SERVER] Socket disconnected:" << web_socket;
  if (web_socket) {
    ClientId client_id = server_model_.GetClientIdByWebSocket(web_socket);
    this->AddEventToHandle(Event(EventType::kClientDisconnected,
                                 client_id));
  }
}

void ServerController::SendToClient(int client_id,
                                    const Event& event) {
  auto client_ptr = server_model_.GetClientByClientId(client_id);
  if (client_ptr) {
    try {
      client_ptr->socket->sendBinaryMessage(event.ToByteArray());
    } catch (std::exception& e) {
      qInfo() << "[SERVER] Caught exception" << e.what();
    }
  }
}

void ServerController::SendToRoom(int room_id,
                                  const Event& event) {
  server_model_.GetRoomByRoomId(room_id)->AddEventToHandle(event);
}

void ServerController::ClientDisconnectedEvent(const Event& event) {
  auto client_id = event.GetArg<ClientId>(0);
  auto room_ptr = server_model_.GetRoomByClientId(client_id);
  room_ptr->RemoveClient(client_id);
  server_model_.RemoveClient(client_id);
}

void ServerController::SetClientsPlayerIdEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ServerController::SendEventToClientEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ServerController::SendEventToRoomEvent(const Event& event) {
  this->AddEventToSend(event);
}

QString ServerController::GetControllerName() const {
  return "SERVER";
}
