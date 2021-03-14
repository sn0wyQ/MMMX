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

  timer_for_keys_ = new QTimer(this);
  connect(timer_for_keys_, &QTimer::timeout, this,
          &ClientController::ApplyDirection);
  timer_for_keys_->start(5);
}

GameDataModel* ClientController::GetModel() {
  return &model_;
}

bool ClientController::IsGameInProgress() const {
  return game_state_ == GameState::kGameInProgress;
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

void ClientController::OnByteArrayReceived(const QByteArray& message) {
  this->AddEventToHandle(Event(message));
}

void ClientController::AddNewPlayerEvent(const Event& event) {
  model_.AddPlayer(event.GetArg<GameObjectId>(1));
  view_->Update();
}

void ClientController::EndGameEvent(const Event& event) {
  game_state_ = GameState::kFinished;
  view_->Update();
}

void ClientController::SetClientsPlayerIdEvent(const Event& event) {
  model_.SetOwnersPlayerId(event.GetArg<GameObjectId>(1));
  qInfo().noquote() << "[CLIENT] Set player_id to"
                    << event.GetArg<GameObjectId>(1);
}

void ClientController::SharePlayersInRoomInfoEvent(const Event& event) {
  for (int i = 2; i < event.GetArg<int>(1) * 3 + 2; i += 3) {
    qInfo() << event.GetArg<int>(i)
            << event.GetArg<float>(i + 1)
            << event.GetArg<float>(i + 2);
    model_.AddPlayer(event.GetArg<GameObjectId>(i),
                     event.GetArg<float>(i + 1),
                     event.GetArg<float>(i + 2));
  }
  view_->Update();
}

void ClientController::StartGameEvent(const Event& event) {
  game_state_ = GameState::kGameInProgress;
  view_->Update();
  qInfo().noquote().nospace() << "[CLIENT] Started game";
}

void ClientController::SendEvent(const Event& event) {
  BaseController::LogEvent(event);
  web_socket_.sendBinaryMessage(event.ToByteArray());
}

void ClientController::OnTick() {}

void ClientController::PlayerDisconnectedEvent(const Event& event) {
  model_.DeletePlayer(event.GetArg<GameObjectId>(0));
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

void ClientController::SendDirectionInfoEvent(const Event& event) {
  this->AddEventToSend(event);
}

void ClientController::UpdatedPlayerPositionEvent(const Event& event) {
  auto player_ptr = model_.GetPlayerByPlayerId(event.GetArg<GameObjectId>(0));
  player_ptr->SetX(event.GetArg<float>(1));
  player_ptr->SetY(event.GetArg<float>(2));
  view_->Update();
}

// -------------------- CONTROLS --------------------

void ClientController::KeyPressEvent(QKeyEvent* key_event) {
  is_direction_by_keys_[key_to_direction_[key_event->nativeVirtualKey()]]
      = true;
}

void ClientController::KeyReleaseEvent(QKeyEvent* key_event) {
  is_direction_by_keys_[key_to_direction_[key_event->nativeVirtualKey()]]
      = false;
}

void ClientController::MouseMoveEvent(QMouseEvent* mouse_event) {

}

void ClientController::ApplyDirection() {
  is_direction_applied_[Direction::kUp]
      = is_direction_applied_[Direction::kRight]
      = is_direction_applied_[Direction::kDown]
      = is_direction_applied_[Direction::kLeft] = false;
  bool is_up_pressed
      = is_direction_by_keys_[Direction::kUp];
  bool is_right_pressed
      = is_direction_by_keys_[Direction::kRight];
  bool is_down_pressed
      = is_direction_by_keys_[Direction::kDown];
  bool is_left_pressed
      = is_direction_by_keys_[Direction::kLeft];
  if ((is_up_pressed ^ is_down_pressed) == 1) {
    is_direction_applied_[is_up_pressed ? Direction::kUp : Direction::kDown]
        = true;
  }
  if ((is_right_pressed ^ is_left_pressed) == 1) {
    is_direction_applied_[is_right_pressed ?
                          Direction::kRight : Direction::kLeft] = true;
  }

  uint32_t mask = is_direction_applied_[Direction::kUp] * 8
      + is_direction_applied_[Direction::kRight] * 4
      + is_direction_applied_[Direction::kDown] * 2
      + is_direction_applied_[Direction::kLeft];

  if (mask != 0) {
    this->AddEventToHandle(Event(EventType::kSendDirectionInfo,
                                 GetModel()->GetOwnersPlayerId(),
                                 mask));
  }
}
