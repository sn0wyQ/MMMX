#include "client_view.h"

ClientView::ClientView(std::shared_ptr<ClientController> controller)
    : AbstractClientView(),
      controller_(std::move(controller)) {
  model_ = controller_->GetModel();

  this->resize(700, 700);
  this->setMinimumSize(310, 70);
  this->setWindowTitle(Constants::kWindowTitle);
  this->setMouseTracking(true);
  this->setFocusPolicy(Qt::StrongFocus);

  game_view_ = new GameView(this, controller_);
  main_menu_ = new MainMenu(this);

  stacked_widget_ = new QStackedWidget(this);
  stacked_widget_->addWidget(game_view_);
  stacked_widget_->addWidget(main_menu_);
  stacked_widget_->setCurrentWidget(main_menu_);
  this->setCentralWidget(stacked_widget_);

  controller_->SetView(std::shared_ptr<ClientView>(this));
}

void ClientView::ConnectToRoom(RoomId room_id) {
  controller_->ConnectToRoom(room_id);
}

void ClientView::SetWindow(ClientWindowType window_type) {
  switch (window_type) {
    case ClientWindowType::kMainMenu:
      stacked_widget_->setCurrentWidget(main_menu_);
      break;

    case ClientWindowType::kGameView:
      stacked_widget_->setCurrentWidget(game_view_);
      break;
  }
}

void ClientView::Update() {
  this->update();
}

std::shared_ptr<Converter> ClientView::GetConverter() {
  return game_view_->GetConverter();
}

void ClientView::focusOutEvent(QFocusEvent* focus_event) {
  controller_->FocusOutEvent(focus_event);
}

void ClientView::keyPressEvent(QKeyEvent* key_event) {
  controller_->KeyPressEvent(key_event);
}

void ClientView::keyReleaseEvent(QKeyEvent* key_event) {
  controller_->KeyReleaseEvent(key_event);
}

void ClientView::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void ClientView::mousePressEvent(QMouseEvent* mouse_event) {
  controller_->MousePressEvent(mouse_event);
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
