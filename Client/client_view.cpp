#include "client_view.h"

ClientView::ClientView(std::shared_ptr<ClientController> controller)
    : AbstractClientView(), controller_(std::move(controller)) {
  resize(700, 700);
  setMinimumSize(310, 70);
  setWindowTitle(Constants::kWindowTitle);
  setMouseTracking(true);

  converter_ = std::make_shared<Converter>(this);

  controller_->SetView(std::shared_ptr<ClientView>(this));
}

void ClientView::Update() {
  this->update();
  repaint();
  qDebug().noquote().nospace() << "[VIEW] Updated";
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
  auto model = controller_->GetModel();
  Painter painter(this,
                  converter_,
                  model->IsOwnersPlayerSet()
                    ? model->GetOwnersPlayer()->GetPosition()
                    : QPointF(0, 0));
  std::vector<std::shared_ptr<Player>> players = model->GetPlayers();
  for (const auto& player : players) {
    player->Draw(&painter);
  }
}

void ClientView::resizeEvent(QResizeEvent* resize_event) {
  converter_->Update(static_cast<float>(resize_event->size().height()));
}
