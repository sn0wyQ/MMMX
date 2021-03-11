#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url) {
  qInfo().noquote().nospace() << "[CLIENT] Connecting to" << url.host();
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
  qInfo().noquote().nospace() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote().nospace() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::ReceiveEvent(const Event& controls_event) {
  this->AddEventToHandle(controls_event);
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::AddNewPlayerEvent(const Event& event) {
  model_.AddPlayer(event.GetArg(1));
}

void ClientController::ChangedTestCounterEvent(const Event& event) {
  auto player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
  player_ptr->SetTestCounterLastDeltaValue(event.GetArg(1));
  player_ptr->SetTestCounterValue(event.GetArg(2));
  view_->Update();
}

void ClientController::EndGameEvent(const Event& event) {
  this->SetGameState(GameState::kFinished);
  view_->Update();
}

void ClientController::PressedTestButtonEvent(const Event& event) {
  this->AddEventToSend(Event(EventType::kPressedTestButton, event.GetArgs()));
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_.SetOwnersPlayerId(event.GetArg(1));
  qInfo().noquote().nospace() << "[CLIENT] Set player_id to" << event.GetArg(1);
}

void ClientController::SharePlayersInRoomIdsEvent(const Event& event) {
  for (int i = 2; i < event.GetArg(1) + 2; i++) {
    model_.AddPlayer(event.GetArg(i));
  }
}

void ClientController::StartGameEvent(const Event& event) {
  this->SetGameState(GameState::kInProgress);
  view_->Update();
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::SendEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::OnTick() {}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_.DeletePlayer(event.GetArg(0));
}

void ClientController::SetView(std::shared_ptr<AbstractClientView> view) {
  view_ = std::move(view);
}

QString ClientController::GetControllerName() const {
  return "CLIENT";
}
