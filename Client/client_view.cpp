#include "client_view.h"

ClientView::ClientView(std::shared_ptr<ClientController> controller)
    : AbstractClientView(),
      controller_(std::move(controller)) {
  resize(700, 700);
  setMinimumSize(310, 70);
  setWindowTitle(Constants::kWindowTitle);
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);

  // Game View
  game_view_ = new GameView(this, controller_->GetModel());
  game_view_->move(0, 0);
  game_view_->setMouseTracking(true);

  // Info Label
  info_label_ = new QLabel(this);
  info_label_->move(10, 10);
  info_label_->setAlignment(Qt::AlignTop);

  controller_->SetView(std::shared_ptr<ClientView>(this));
  model_ = controller_->GetModel();
}

void ClientView::Update() {
  this->update();
  game_view_->update();
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

void ClientView::paintEvent(QPaintEvent* paint_event) {
  auto local_player = model_->IsLocalPlayerSet()
                      ? model_->GetLocalPlayer()
                      : std::make_shared<Player>(Constants::kNullGameObjectId);

  info_label_->setText(QString(tr("Server Var: %1\nPing: %2\nX: %3,  \tY: %4"))
                       .arg(controller_->GetServerVar())
                       .arg(controller_->GetPing())
                       .arg(local_player->GetX())
                       .arg(local_player->GetY()));
  info_label_->adjustSize();

  qDebug().noquote().nospace() << "[VIEW] Repainted";
}

void ClientView::resizeEvent(QResizeEvent* resize_event) {
  game_view_->resize(resize_event->size());
}
