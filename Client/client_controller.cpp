#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url) {
  qInfo() << "[CLIENT] Connecting to" << url;
  connect(&web_socket_,
          &QWebSocket::connected,
          this,
          &ClientController::OnConnected);
  connect(&web_socket_,
          &QWebSocket::disconnected,
          this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
}

void ClientController::HandleEvent(const Event& event) {
  switch (event.GetType()) {
    case EventType::kAddNewPlayer: {
      model_.AddPlayer(event.GetArg(1));
      break;
    }

    case EventType::kChangedTestCounter: {
      auto player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
      player_ptr->SetTestCounterLastDeltaValue(event.GetArg(1));
      player_ptr->SetTestCounterValue(event.GetArg(2));
      emit(UpdateView());
      break;
    }

    case EventType::kEndGame: {
      this->SetGameState(GameState::kFinished);
      emit(UpdateView());
      break;
    }

    case EventType::kPressedTestButton: {
      this->AddEventToSend(EventType::kPressedTestButton, event.GetArgs());
      break;
    }

    case EventType::kSetClientsPlayerId: {
      model_.SetOwnersPlayerId(event.GetArg(1));
      qInfo() << "[CLIENT] Set player_id to" << event.GetArg(1);
      break;
    }

    case EventType::kSharePlayersInRoomIds: {
      for (int i = 2; i < event.GetArg(1) + 2; i++) {
        model_.AddPlayer(event.GetArg(i));
      }
      break;
    }

    case EventType::kStartGame: {
      this->SetGameState(GameState::kInProgress);
      emit(UpdateView());
      qInfo() << "[CLIENT] Started game";
      break;
    }

    default:
      break;
  }
}

void ClientController::Send() {
  while (this->HasEventsToSend()) {
    Event current_event = this->GetNextEventToSend();
    web_socket_.sendBinaryMessage(current_event.ToByteArray());
    qInfo() << "[CLIENT] Sent" << current_event << "to Server";
  }
}

GameDataModel* ClientController::GetModel() {
  return &model_;
}

void ClientController::SetGameState(GameState game_state) {
  game_state_ = game_state;
}

bool ClientController::IsInProgress() const {
  return game_state_ == GameState::kInProgress;
}

void ClientController::OnConnected() {
  connect(&web_socket_,
          &QWebSocket::binaryMessageReceived,
          this,
          &ClientController::OnByteArrayReceived);

  // TODO(Everyone): Send nickname to server after connection
  this->StartTicking();
  qInfo() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::OnControlsEventReceived(const Event& controls_event) {
  this->AddEventToHandle(controls_event);
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(message);

  qInfo() << "[CLIENT] Received" << Event(message) << "from Server";
}
