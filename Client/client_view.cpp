#include "client_view.h"

ClientView::ClientView(
    std::shared_ptr<ClientController> controller)
    : AbstractClientView(), controller_(std::move(controller)),
    timer_for_keys_(new QTimer()) {
  resize(700, 700);
  setMinimumSize(310, 70);
  setWindowTitle(Constants::kWindowTitle);

  controller_->SetView(std::shared_ptr<ClientView>(this));

  connect(timer_for_keys_, &QTimer::timeout, this,
          &ClientView::ApplyDirection);
  timer_for_keys_->start(Constants::kTimeToUpdateKeys);
}

void ClientView::Update() {
  this->update();
  repaint();
  // qInfo().noquote().nospace() << "[VIEW] Updated";
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  QPainter painter(this);
  std::vector<std::shared_ptr<Player>> players
    = controller_->GetModel()->GetPlayers();
  for (const auto& player : players) {
    painter.drawEllipse(player->GetPosition(), 50, 50);
  }
  painter.drawText(0, 50, "svar : "
  + QString::number(controller_->GetSvar()));
  painter.drawText(0, 70, "ping : "
  + QString::number(controller_->GetQPing()));
}

void ClientView::keyPressEvent(QKeyEvent* key_event) {
  int native_key = key_event->nativeVirtualKey();
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]]
        = true;
  }
  if (key_event->nativeVirtualKey() == Qt::Key_Alt) {
    ResetDirection();
  }
}

void ClientView::keyReleaseEvent(QKeyEvent* key_event) {
  int native_key = key_event->nativeVirtualKey();
  if (key_to_direction_.find(native_key) != key_to_direction_.end()) {
    is_direction_by_keys_[key_to_direction_[native_key]]
        = false;
  }
}

void ClientView::ApplyDirection() {
  ResetDirection();
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

  int mask = is_direction_applied_[Direction::kUp] * 8
      + is_direction_applied_[Direction::kRight] * 4
      + is_direction_applied_[Direction::kDown] * 2
      + is_direction_applied_[Direction::kLeft];
  if (mask != 0) {
    controller_->ReceiveEvent(
        Event(EventType::kSendDirectionInfo,
              controller_->GetModel()->GetOwnersPlayerId(),
              mask));
  }
  // qInfo() << is_direction_applied_[Direction::kUp]
  // << is_direction_applied_[Direction::kRight]
  // << is_direction_applied_[Direction::kDown]
  // << is_direction_applied_[Direction::kLeft];
}

void ClientView::ResetDirection() {
  is_direction_applied_[Direction::kUp]
      = is_direction_applied_[Direction::kRight]
      = is_direction_applied_[Direction::kDown]
      = is_direction_applied_[Direction::kLeft] = false;
}
