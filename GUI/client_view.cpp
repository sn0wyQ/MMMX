#include "client_view.h"

ClientView::ClientView(ClientController* controller)
    : AbstractClientView(),
      controller_(controller),
      model_(controller_->GetModel()) {
  this->setMinimumSize(1400, 960);
  this->setWindowTitle(Constants::kWindowTitle);
  this->setFocusPolicy(Qt::NoFocus);

  game_view_ = new GameView(this, controller_);
  game_view_->setMouseTracking(true);
  key_controller_ = game_view_->GetKeyController();

  main_menu_ = new MainMenu(this, controller_);

  settings_window_ = new SettingsWindow(this, controller_);

  stacked_widget_ = new QStackedWidget(this);
  stacked_widget_->addWidget(game_view_);
  stacked_widget_->addWidget(main_menu_);
  stacked_widget_->addWidget(settings_window_);
  stacked_widget_->setCurrentWidget(main_menu_);
  this->setCentralWidget(stacked_widget_);

  controller_->SetView(this);

  this->resize(1400, 960);
}

void ClientView::ConnectToRoom(RoomId room_id, PlayerType player_type) {
  controller_->ConnectToRoom(room_id, player_type);
}

void ClientView::SetWindow(ClientWindowType window_type) {
  switch (window_type) {
    case ClientWindowType::kMainMenu:
      stacked_widget_->setCurrentWidget(main_menu_);
      break;

    case ClientWindowType::kGameView:
      stacked_widget_->setCurrentWidget(game_view_);
      break;

    case ClientWindowType::kSettingsWindow:
      stacked_widget_->setCurrentWidget(settings_window_);
      break;
  }
}

void ClientView::Update() {
  game_view_->Update();
}

void ClientView::UpdateRoomsInfoList() {
  main_menu_->UpdateRoomsInfoList();
}

std::shared_ptr<Converter> ClientView::GetConverter() {
  return game_view_->GetConverter();
}

void ClientView::focusOutEvent(QFocusEvent* focus_event) {
  controller_->FocusOutEvent(focus_event);
}

void ClientView::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void ClientView::mousePressEvent(QMouseEvent* mouse_event) {
  controller_->MousePressEvent(mouse_event);
}

void ClientView::closeEvent(QCloseEvent* close_event) {
  this->QuitApp();
}

void ClientView::paintEvent(QPaintEvent* paint_event) {
  qDebug().noquote().nospace() << "[VIEW] Repainted";
}

void ClientView::resizeEvent(QResizeEvent* resize_event) {
  qDebug().noquote().nospace() << "[VIEW] Resized";
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
  game_view_->AddKillNotification(killer_name, victim_name, weapon_type);
}

void ClientView::AddRespawnNotification(const QString& player_name) {
  game_view_->AddRespawnNotification(player_name);
}

void ClientView::AddPlayerConnectedNotification(const QString& player_name) {
  game_view_->AddPlayerConnectedNotification(player_name);
}

void ClientView::AddPlayerDisconnectedNotification(const QString& player_name) {
  game_view_->AddPlayerDisconnectedNotification(player_name);
}

KeyController* ClientView::GetKeyController() const {
  return key_controller_;
}

void ClientView::QuitApp() {
  controller_->deleteLater();
  this->deleteLater();
}

void ClientView::SetFullScreen() {
  if (this->isFullScreen()) {
    return;
  }

  this->setWindowState(Qt::WindowFullScreen);
}

void ClientView::SetWindowed() {
  if (!this->isFullScreen()) {
    return;
  }

  this->setWindowState(Qt::WindowNoState);
}

void ClientView::ResetScreenState() {
  if (Settings::GetInstance().GetValueByKey<bool>("main/fullscreen")) {
    this->SetFullScreen();
  } else {
    this->SetWindowed();
  }
}
