#include "server_controller.h"

ServerController::ServerController()
  : web_socket_server_(Constants::kServerName,
                       QWebSocketServer::NonSecureMode) {
  if (web_socket_server_.listen(QHostAddress::Any,  Constants::kServerPort)) {
    qInfo().noquote().nospace() << "Server is running on port "
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

    // If we're here something most likely went wrong
    default:
        qWarning() << "[SERVER] Impossible to send event" << event;
      break;
  }
}

void ServerController::OnTick(int delta_time) {
  std::vector<RoomId> rooms_to_delete;
  for (const auto& [room_id, room] : server_model_.GetRooms()) {
    if (!room->HasPlayers() && !room->IsWaitingForClients()) {
      rooms_to_delete.push_back(room_id);
    }
  }

  for (const auto& room_id : rooms_to_delete) {
    server_model_.DeleteRoom(room_id);
  }

  int number_of_rooms_to_create =
      Constants::ServerController::kMinimumNumberOfPublicRoomsWithFreePlace;
  for (const auto& [room_id, room] : server_model_.GetRooms()) {
    if (room->IsWaitingForClients() && room->IsPublic()) {
      if (room->HasFreeSpot()) {
        --number_of_rooms_to_create;
      } else {
        room->StartGame();
      }
    }
  }

  while (number_of_rooms_to_create--) {
    server_model_.AddNewRoom();
  }

  this->ProcessEventsFromRooms();

  for (auto& [client_id, cache] : event_cache_) {
    auto client_ptr = server_model_.GetClientByClientId(client_id);
    if (client_ptr) {
      try {
        client_ptr->socket->sendBinaryMessage(cache.ToByteArray());
      } catch (std::exception& e) {
        qInfo() << "[SERVER] Caught exception" << e.what();
      }
    }
  }
  event_cache_.clear();
}

void ServerController::ProcessEventsFromRooms() {
  auto rooms = server_model_.GetRooms();
  for (const auto& room : rooms) {
    this->ProcessEventsFromRoom(room.second);
  }
}

void ServerController::ProcessEventsFromRoom(
    const std::shared_ptr<RoomController>& room_ptr) {
  std::vector<Event> events_from_room = room_ptr->ClaimEventsForServer();
  for (const auto& event : events_from_room) {
    this->AddEventToHandle(event);
  }
}

void ServerController::OnByteArrayReceived(const QByteArray& message) {
  auto client_socket_ptr = qobject_cast<QWebSocket*>(sender());
  auto client_id = server_model_.GetClientIdByWebSocket(client_socket_ptr);
  auto client = server_model_.GetClientByClientId(client_id);
  if (!client) {
    return;
  }
  auto room_id = client->room_id;

  auto events = PackedEvent(message).GetEvents();
  for (const auto& event : events) {
    qDebug().noquote() << "[SERVER] Received" << event
                       << "from Client ID:" << client_id;

    if (event.GetType() == EventType::kSetTimeDifference) {
      // Важная каждая миллисекунда - отправляем ответ сразу без тика
      this->ReplyWithTimeToClient(client_id, event.GetArg<int64_t>(0));
      return;
    }

    if (room_id == Constants::kNullRoomId) {
      if (event.GetType() == EventType::kConnectToRoomById) {
        auto new_room_id = event.GetArg<RoomId>(0);
        auto player_type = event.GetArg<PlayerType>(1);
        auto rooms = server_model_.GetRooms();
        if (rooms.find(new_room_id) == rooms.end()) {
          return;
        }
        auto nickname = event.GetArg<QString>(2);
        server_model_.AddClientToRoom(
            new_room_id, client_id, nickname, player_type);
        client->room_id = new_room_id;
        client->nickname = nickname;
      }
      return;
    }

    if (event.GetType() == EventType::kDisconnectFromRoom) {
      auto rooms = server_model_.GetRooms();
      auto room_iter = rooms.find(client->room_id);
      if (room_iter != rooms.end()) {
        room_iter->second->RemoveClient(client_id);
      }
      client->room_id = Constants::kNullRoomId;

      // We need to wait a bit,
      // because sometimes server can not update server list fast enough
      QTimer::singleShot(50, this, [this, client_id] () {
        this->SendVisibleRoomsInfo(client_id);
      });
      return;
    }

    if (event.GetType() == EventType::kSendGetVars) {
      this->AddEventToHandle(Event(EventType::kSendEventToClient,
                                   client_id,
                                   static_cast<int>(EventType::kUpdateVars),
                                   this->GetVar(),
                                   server_model_.GetRoomByClientId(client_id)
                                       ->GetVar()));
      return;
    }

    std::vector<QVariant>
        args{room_id, static_cast<int>(event.GetType())};
    std::vector<QVariant> old_args = event.GetArgs();
    args.insert(args.end(), old_args.begin(), old_args.end());

    this->AddEventToHandle(Event(EventType::kSendEventToRoom, args));
  }
}

void ServerController::OnNewClient() {
  std::shared_ptr<QWebSocket> current_socket(
      web_socket_server_.nextPendingConnection());

  // TODO(Everyone): some mechanism to restore client state if it had
  //  disconnected during last X seconds

  ClientId new_client_id = server_model_.GetNextUnusedClientId();

  auto new_client =
      std::make_shared<ServerModel::ConnectedClient>(current_socket);

  server_model_.SetConnectedClient(new_client_id, new_client);
  server_model_.SetClientIdToWebSocket(current_socket, new_client_id);

  connect(new_client->socket.get(),
          &QWebSocket::binaryMessageReceived,
          this,
          &ServerController::OnByteArrayReceived);

  connect(new_client->socket.get(),
          &QWebSocket::disconnected,
          this,
          &ServerController::OnSocketDisconnected);

  this->SendVisibleRoomsInfo(new_client_id);
  qInfo().noquote().nospace() << "[SERVER] New Client: ID - " << new_client_id;
}

void ServerController::SendVisibleRoomsInfo(ClientId client_id) {
  auto visible_rooms_info = server_model_.GetVisibleRoomsInfo(client_id);
  this->AddEventToSend(Event(EventType::kSendEventToClient,
                             client_id,
                             static_cast<int>(EventType::kSendVisibleRoomsInfo),
                             visible_rooms_info));
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

void ServerController::SendToClient(int client_id, const Event& event) {
  event_cache_[client_id].AddEvent(event);
}

void ServerController::SendToRoom(int room_id,
                                  const Event& event) {
  auto rooms = server_model_.GetRooms();
  auto room_iter = rooms.find(room_id);
  if (room_iter != rooms.end()) {
    room_iter->second->AddEventToHandle(event);
  } else {
    qWarning() << "Trying to send event to removed room";
  }
}

void ServerController::ClientDisconnectedEvent(const Event& event) {
  try {
    auto client_id = event.GetArg<ClientId>(0);
    if (server_model_.GetRoomIdByClientId(client_id)
        != Constants::kNullRoomId) {
      auto room_ptr = server_model_.GetRoomByClientId(client_id);
      room_ptr->RemoveClient(client_id);
    }
    server_model_.RemoveClient(client_id);
  } catch(std::exception& e) {
    qWarning() << e.what() << "Maybe client was already removed?";
  }
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

void ServerController::ReplyWithTimeToClient(
    ClientId client_id, int64_t client_time) {
  Event new_event(EventType::kSetTimeDifference,
                  static_cast<qint64>(client_time),
                  static_cast<qint64>(this->GetCurrentServerTime()));
  this->SendToClient(client_id, new_event);
}
