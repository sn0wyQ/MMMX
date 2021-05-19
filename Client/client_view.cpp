#include "client_view.h"

ClientView::ClientView(std::shared_ptr<ClientController> controller)
    : AbstractClientView(),
      controller_(std::move(controller)) {
  resize(1400, 960);
  height_of_bar_ = static_cast<int>(
      Constants::kPlayerBarHeightRatio * static_cast<float>(height()));

  setMinimumSize(1300, 700);
  setWindowTitle(Constants::kWindowTitle);
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);

  // Game View
  game_view_ = new GameView(this, controller_->GetModel());
  game_view_->move(0, 0);
  game_view_->setMouseTracking(true);

  // Kill feed
  kill_feed_ = new KillFeed(this);

  // Player Bar
  player_bar_ = new PlayerBar(this, controller_->GetModel(),
                              QPoint(0, height() - height_of_bar_),
                              QSize(width(), height_of_bar_));

  // Info Label
  info_label_ = new QLabel(this);
  info_label_->move(10, 10);
  info_label_->setAlignment(Qt::AlignTop);

  // Reloading Field
  reloading_field_ = new ReloadingField(this, controller_);

  // Respawn Button
  respawn_button_ = new RespawnButton(this);
  respawn_button_->Hide();

  // Stats table
  stats_table_ = new StatsTable(this, controller_->GetModel());
  stats_table_->setMouseTracking(true);

  // Controls Settings
  key_controller_ = std::make_shared<KeyController>(this);
  key_controller_->Hide();

  controller_->SetView(std::shared_ptr<ClientView>(this));
  model_ = controller_->GetModel();

  resize(1400, 960);
}

void ClientView::Update() {
  auto time = QDateTime::currentMSecsSinceEpoch();
  if (last_frame_times_.empty()) {
    last_updated_time_ = time;
  }
  last_frame_times_.push_back(time - last_updated_time_);
  if (static_cast<int>(last_frame_times_.size()) >
      Constants::kAverageFpsFrames) {
    last_frame_times_.pop_front();
  }
  last_updated_time_ = time;

  game_view_->Update();
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
  if (key_controller_->IsShown()) {
    if (key_event->key() == Qt::Key_F1 || key_event->key() == Qt::Key_Escape) {
      key_controller_->Hide();
    }
  } else if (key_event->key() == Qt::Key_F1) {
    key_controller_->Show();
  }
  if (key_controller_->IsShown()) {
    key_controller_->keyPressEvent(key_event);
  }
  controller_->KeyPressEvent(key_event);
}

void ClientView::keyReleaseEvent(QKeyEvent* key_event) {
  controller_->KeyReleaseEvent(key_event);
}

void ClientView::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void ClientView::mousePressEvent(QMouseEvent* mouse_event) {
  if (mouse_event->button() == Qt::MouseButton::LeftButton &&
      key_controller_->IsShown()) {
    key_controller_->Hide();
  }
  if (key_controller_->IsShown()) {
    key_controller_->mousePressEvent(mouse_event);
  }
  controller_->MousePressEvent(mouse_event);
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  if (key_controller_->IsHeld(Controls::kShowStatistics)) {
    stats_table_->Show();
  } else {
    stats_table_->Hide();
  }

  this->ProcessRespawnButton();

  auto local_player_position = model_->IsLocalPlayerSet()
                               ? model_->GetLocalPlayer()->GetPosition()
                               : QPointF(0.f, 0.f);
  int64_t average_frame_time = 0;
  int64_t fps = 0;
  if (!last_frame_times_.empty()) {
    // Weird maths with divide to avoid accuracy loss
    average_frame_time = std::accumulate(
        last_frame_times_.begin(), last_frame_times_.end(), 0LL);
    fps = static_cast<int64_t>(last_frame_times_.size()) * 1000 /
        (average_frame_time + 1);
    average_frame_time /= static_cast<int64_t>(last_frame_times_.size());
  }

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
                           .arg(average_frame_time));
  info_label_->adjustSize();

  qDebug().noquote().nospace() << "[VIEW] Repainted";
}

void ClientView::resizeEvent(QResizeEvent* resize_event) {
  respawn_button_->Resize(QSize(150, 150));
  respawn_button_default_position_ =
      QPoint(10,
             this->height() - height_of_bar_ - respawn_button_->height() - 10);
  respawn_button_->Move(respawn_button_default_position_);
  game_view_->resize(resize_event->size());
  player_bar_->resize(width(), height_of_bar_);
  player_bar_->move(0, height() - height_of_bar_);
  stats_table_->Resize(QSize(this->width() * 0.9f,
                             (this->height() - height_of_bar_) * 0.9f));
  stats_table_->move(
      (this->width() - stats_table_->width()) / 2.f,
      (this->height() - stats_table_->height() - height_of_bar_) / 2.f);

  reloading_field_->resize(this->width() / 3, this->height() / 2);
  reloading_field_->move(this->width() - reloading_field_->width(),
                         (this->height() - height_of_bar_
                             - reloading_field_->height()));

  kill_feed_->resize(this->width() / 3, this->height());
  kill_feed_->move(this->width() - kill_feed_->width(), 0);
  key_controller_->move(this->width() / 4, this->height() / 4);
  key_controller_->resize(this->width() / 2, this->height());
}

void ClientView::mouseReleaseEvent(QMouseEvent* mouse_event) {
  controller_->MouseReleaseEvent(mouse_event);
}

QPointF ClientView::GetPlayerToCenterOffset() const {
  return game_view_->GetPlayerToCenterOffset();
}

void ClientView::AddKillFeedNotification(const QString& killer_name,
                                         const QString& victim_name,
                                         WeaponType weapon_type) {
  kill_feed_->AddKillNotification(killer_name, victim_name, weapon_type);
}

void ClientView::AddRespawnNotification(const QString& player_name) {
  kill_feed_->AddSpawnNotification(player_name);
}

void ClientView::AddPlayerConnectedNotification(const QString& player_name) {
  kill_feed_->AddPlayerConnectedNotification(player_name);
}

void ClientView::AddPLayerDisconnectedNotification(const QString& player_name) {
  kill_feed_->AddPLayerDisconnectedNotification(player_name);
}

void ClientView::ProcessRespawnButton() {
  respawn_button_->SetWaitValue(controller_->GetSecsToNextPossibleRevive());
  respawn_button_->SetValue(controller_->GetHoldingRespawnButtonMsecs());
  if (controller_->GetIsHoldingRespawnButton() ||
      controller_->GetHoldingRespawnButtonMsecs() != 0) {
    respawn_button_->Show();
  } else {
    respawn_button_->Hide();
  }
  if (model_->IsLocalPlayerSet() && model_->GetLocalPlayer()->IsAlive()) {
    respawn_button_->Move(respawn_button_default_position_);
  } else {
    respawn_button_->Move(QPointF(
        this->width() / 2 - respawn_button_->width() / 2,
        (this->height() - height_of_bar_) / 2 - respawn_button_->height() / 2));
  }
}

std::shared_ptr<KeyController> ClientView::GetKeyController() const {
  return std::shared_ptr<KeyController>(key_controller_);
}
