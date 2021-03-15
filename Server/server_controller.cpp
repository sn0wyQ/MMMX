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

ServerController::~ServerController() {
  web_socket_server_.close();
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
        qWarning() << "[SERVER] Impossible to send event " << event;
      break;
  }
}

void ServerController::OnTick() {
  for (auto& room_iter : server_model_.GetRooms()) {
    if (!room_iter.second->HasPlayers()
        && !room_iter.second->IsWaitingForClients()) {
      server_model_.DeleteRoom(room_iter.second->GetId());
    }
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

  bool found_existing_room_with_a_free_place = false;
  while (!found_existing_room_with_a_free_place
      && !server_model_.IsRoomsQueueEmpty()) {
    bool need_to_pop_room = true;

    if (server_model_.IsTopRoomInQueueExistsInModel()) {
      auto top_room = server_model_.GetTopRoomInQueue();
      if (top_room->HasFreeSpot()
          && top_room->IsWaitingForClients()) {
        room_id = top_room->GetId();
        found_existing_room_with_a_free_place = true;
        need_to_pop_room = false;
      }
    }

    if (need_to_pop_room) {
      server_model_.PopTopRoomInQueue();
    }
  }

  if (!found_existing_room_with_a_free_place) {
    room_id = server_model_.AddNewRoom();
    server_model_.AddToRoomsWithFreeSpot(room_id);
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
  try {
    auto client_ptr = server_model_.GetClientByClientId(client_id);
    client_ptr->socket->sendBinaryMessage(event.ToByteArray());
  } catch (std::exception& e) {
    qInfo() << "[SERVER] Caught exception" << e.what();
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
