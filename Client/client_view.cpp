#include "client_view.h"

ClientView::ClientView(std::shared_ptr<ClientController> controller)
    : AbstractClientView(),
      controller_(std::move(controller)) {
  resize(1400, 960);
  height_of_bar_ = static_cast<int>(0.15f * static_cast<float>(height()));
  setMinimumSize(1300, 700);
  setWindowTitle(Constants::kWindowTitle);
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);

  // Game View
  game_view_ = new GameView(this, controller_->GetModel());
  game_view_->move(0, 0);
  game_view_->setMouseTracking(true);

  // Player Bar
  player_bar_ = new PlayerBar(this, controller_->GetModel(),
                              QPoint(0, height() - height_of_bar_),
                              QSize(width(), height_of_bar_));

  // Info Label
  info_label_ = new QLabel(this);
  info_label_->move(10, 10);
  info_label_->setAlignment(Qt::AlignTop);

  // Stats table
  stats_table_ = new StatsTable(this, controller_->GetModel());
  stats_table_->setMouseTracking(true);
  stats_table_->Hide();

  controller_->SetView(std::shared_ptr<ClientView>(this));
  model_ = controller_->GetModel();
}

void ClientView::Update() {
  this->update();
}

std::shared_ptr<Converter> ClientView::GetConverter() {
  return game_view_->GetConverter();
}

void ClientView::focusOutEvent(QFocusEvent* focus_event) {
  stats_table_->Hide();
  controller_->FocusOutEvent(focus_event);
}

void ClientView::keyPressEvent(QKeyEvent* key_event) {
  if (key_event->key() == Qt::Key_Tab) {
    stats_table_->Show();
  }
  controller_->KeyPressEvent(key_event);
}

void ClientView::keyReleaseEvent(QKeyEvent* key_event) {
  if (key_event->key() == Qt::Key_Tab) {
    stats_table_->Hide();
  }
  controller_->KeyReleaseEvent(key_event);
}

void ClientView::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void ClientView::mousePressEvent(QMouseEvent* mouse_event) {
  controller_->MousePressEvent(mouse_event);
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  auto local_player_position = model_->IsLocalPlayerSet()
                               ? model_->GetLocalPlayer()->GetPosition()
                               : QPointF(0.f, 0.f);
  auto time = QDateTime::currentMSecsSinceEpoch();
  int frame_time = static_cast<int>(time - last_updated_time_);
  last_frame_times_.push_back(frame_time);
  if (last_frame_times_.size() > Constants::kAverageFpsFrames) {
    last_frame_times_.pop_front();
  }
  last_updated_time_ = time;
  frame_time = std::accumulate(last_frame_times_.begin(),
                               last_frame_times_.end(), 0) /
                                   static_cast<int>(last_frame_times_.size());
  int fps = 1000 / (frame_time + 1);

  info_label_->setText(QString(tr("Server Var: %1\n"
                                  "Room Var: %2\n"
                                  "Client Var: %3\n"
                                  "Ping: %4\n"
                                  "X: %5, \tY: %6\n"
                                  "Fps: %7 (%8ms)\n"))
                           .arg(controller_->GetServerVar())
                           .arg(controller_->GetRoomVar())
                           .arg(controller_->GetClientVar())
                           .arg(controller_->GetPing())
                           .arg(local_player_position.x())
                           .arg(local_player_position.y())
                           .arg(fps)
                           .arg(frame_time));
  info_label_->adjustSize();

  qDebug().noquote().nospace() << "[VIEW] Repainted";
}

void ClientView::resizeEvent(QResizeEvent* resize_event) {
  game_view_->resize(resize_event->size());
  player_bar_->resize(width(), height_of_bar_);
  player_bar_->move(0, height() - height_of_bar_);
  stats_table_->resize(this->width() * 0.9f,
                       (this->height() - height_of_bar_) * 0.9f);
  stats_table_->move(
      (this->width() - stats_table_->width()) / 2.f,
      (this->height() - stats_table_->height() - height_of_bar_) / 2.f);
}

void ClientView::mouseReleaseEvent(QMouseEvent* mouse_event) {
  controller_->MouseReleaseEvent(mouse_event);
}
