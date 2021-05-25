#include "game_view.h"

GameView::GameView(AbstractClientView* parent, ClientController* controller)
    : parent_(parent), controller_(controller) {
  model_ = controller_->GetModel();

  view_port_ = new ViewPort(this, controller_);
  view_port_->move(0, 0);
  view_port_->setMouseTracking(true);

  kill_feed_ = new KillFeed(this);

  height_of_bar_ = static_cast<int>(
      Constants::kPlayerBarHeightRatio * static_cast<float>(parent_->height()));
  player_bar_ = new PlayerBar(this, model_,
                              QPoint(0, height() - height_of_bar_),
                              QSize(width(), height_of_bar_));

  info_label_ = new QLabel(this);
  info_label_->setObjectName("info");
  info_label_->move(10, 10);
  info_label_->setAlignment(Qt::AlignTop);
  info_label_->hide();

  reloading_field_ = new ReloadingField(this, controller_);

  respawn_button_ = new RespawnButton(this);
  respawn_button_->Hide();

  stats_table_ = new StatsTable(this, controller_->GetModel());
  stats_table_->setMouseTracking(true);

  disconnect_button_ = new QPushButton("Disconnect", this);
  disconnect_button_->setFixedHeight(30);
  disconnect_button_->setFocusPolicy(Qt::NoFocus);
  connect(disconnect_button_,
          &QPushButton::clicked,
          this,
          &GameView::OnDisconnectButtonClicked);
  disconnect_button_->setObjectName("small_btn");

  key_controller_ = new KeyController(this);
  key_controller_->Hide();
}

std::shared_ptr<Converter> GameView::GetConverter() {
  return view_port_->GetConverter();
}

void GameView::Update() {
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

  view_port_->Update();
  this->update();
}

void GameView::keyPressEvent(QKeyEvent* key_event) {
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

void GameView::keyReleaseEvent(QKeyEvent* key_event) {
  controller_->KeyReleaseEvent(key_event);
}

void GameView::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void GameView::paintEvent(QPaintEvent* event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

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

  if (key_controller_->WasPressed(Controls::kShowNetGraph)) {
    key_controller_->ClearKeyPress(Controls::kShowNetGraph);
    if (info_label_->isHidden()) {
      info_label_->show();
    } else {
      info_label_->hide();
    }
  }
  if (!info_label_->isHidden()) {
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
  }
}

void GameView::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  respawn_button_->Resize(QSize(150, 150));
  respawn_button_default_position_ =
      QPoint(10,
             this->height() - height_of_bar_ - respawn_button_->height() - 10);
  respawn_button_->Move(respawn_button_default_position_);

  view_port_->resize(event->size());

  player_bar_->resize(width, height_of_bar_);
  player_bar_->move(0, height - height_of_bar_);

  stats_table_->Resize(QSize(width * 0.9f, (height - height_of_bar_) * 0.9f));
  stats_table_->move((width - stats_table_->width()) / 2.f,
                     (height - stats_table_->height() - height_of_bar_) / 2.f);

  reloading_field_->resize(width / 3, height / 2);
  reloading_field_->move(width - reloading_field_->width(),
                         (height - height_of_bar_
                          - reloading_field_->height()));

  kill_feed_->resize(width / 3, height);
  kill_feed_->move(width - kill_feed_->width(), 0);

  disconnect_button_->setGeometry(width * 5 / 12,
                                  height - 35,
                                  width / 6,
                                  30);

  key_controller_->move(width / 4, height / 4);
  key_controller_->resize(width / 2, height);
}

void GameView::ProcessRespawnButton() {
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

QPointF GameView::GetPlayerToCenterOffset() const {
  return view_port_->GetPlayerToCenterOffset();
}

void GameView::AddKillNotification(const QString& killer_name,
                                   const QString& victim_name,
                                   WeaponType weapon_type) {
  kill_feed_->AddKillNotification(killer_name, victim_name, weapon_type);
}

void GameView::AddRespawnNotification(const QString& player_name) {
  kill_feed_->AddSpawnNotification(player_name);
}

void GameView::AddPlayerConnectedNotification(const QString& player_name) {
  kill_feed_->AddPlayerConnectedNotification(player_name);
}

void GameView::AddPlayerDisconnectedNotification(const QString& player_name) {
  kill_feed_->AddPlayerDisconnectedNotification(player_name);
}

void GameView::OnDisconnectButtonClicked() {
  parent_->SetWindow(ClientWindowType::kMainMenu);
  controller_->DisconnectFromRoom();
}

KeyController* GameView::GetKeyController() const {
  return key_controller_;
}
