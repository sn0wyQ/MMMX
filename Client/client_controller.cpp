#include "client_controller.h"

ClientController::ClientController(const QUrl& url) : url_(url) {
  qInfo().noquote() << "[CLIENT] Connecting to" << url.host();
  connect(&web_socket_,
          &QWebSocket::connected,
          this,
          &ClientController::OnConnected);
  connect(&web_socket_,
          &QWebSocket::disconnected,
          this,
          &ClientController::OnDisconnected);
  web_socket_.open(url);
  this->StartTicking();
}

GameDataModel* ClientController::GetModel() {
  return &model_;
}

bool ClientController::IsInProgress() const {
  return game_state_ == GameState::kInProgress;
}

void ClientController::OnConnected() {
  connect(&web_socket_,
          &QWebSocket::binaryMessageReceived,
          this,
          &ClientController::OnByteArrayReceived);
  view_->Update();
  // TODO(Everyone): Send nickname to server after connection
  qInfo().noquote() << "[CLIENT] Connected to" << url_;
}

void ClientController::OnDisconnected() {
  qInfo().noquote() << "[CLIENT] Disconnected from" << url_;
}

void ClientController::ReceiveEvent(const Event& controls_event) {
  this->AddEventToHandle(controls_event);
}

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::AddNewPlayerEvent(const Event& event) {
  model_.AddPlayer(event.GetArg(1));
  view_->Update();
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kFinished;
  view_->Update();
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_.SetOwnersPlayerId(event.GetArg(1));
  qInfo().noquote() << "[CLIENT] Set player_id to" << event.GetArg(1);
}

void ClientController::SharePlayersInRoomInfoEvent(const Event& event) {
  for (int i = 2; i < event.GetArg(1) + 2; i += 3) {
    model_.AddPlayer(event.GetArg(i),
                     event.GetArg(i + 1),
                     event.GetArg(i + 2));
  }
  view_->Update();
}

void ClientController::StartGameEvent(const Event& event) {
  game_state_ = GameState::kInProgress;
  view_->Update();
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::OnTick() {}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_.DeletePlayer(event.GetArg(0));
  game_state_ = GameState::kNotStarted;
  view_->Update();
}

void ClientController::SetView(std::shared_ptr<AbstractClientView> view) {
  view_ = std::move(view);
}

QString ClientController::GetControllerName() const {
  return "CLIENT";
}

// ------------------- GAME EVENTS -------------------

void ClientController::ChangedTestCounterEvent(const Event& event) {
  auto player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
  player_ptr->SetTestCounterLastDeltaValue(event.GetArg(1));
  player_ptr->SetTestCounterValue(event.GetArg(2));
  view_->Update();
}

void ClientController::PressedTestButtonEvent(const Event& event) {
  this->AddEventToSend(Event(EventType::kPressedTestButton,
                             event.GetArgs()));
}

void ClientController::SendDirectionInfoEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ClientController::UpdatedPlayerPositionEvent(const Event& event) {
  auto player_ptr = model_.GetPlayerByPlayerId(event.GetArg(0));
  player_ptr->SetX(event.GetArg(1));
  player_ptr->SetY(event.GetArg(2));
  view_->Update();
}
