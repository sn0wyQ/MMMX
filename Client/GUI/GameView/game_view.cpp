#include "game_view.h"

GameView::GameView(AbstractClientView* parent,
                   std::shared_ptr<ClientController> controller)
    : parent_(parent), controller_(std::move(controller)) {
  model_ = controller_->GetModel();

  view_port_ = new ViewPort(this, model_);
  view_port_->move(0, 0);

  player_bar_ = new PlayerBar(this, model_);

  info_label_ = new QLabel(this);
  info_label_->move(10, 10);
  info_label_->setAlignment(Qt::AlignTop);
}

std::shared_ptr<Converter> GameView::GetConverter() {
  return view_port_->GetConverter();
}

void GameView::paintEvent(QPaintEvent* event) {
  auto local_player_position = model_->IsLocalPlayerSet()
                               ? model_->GetLocalPlayer()->GetPosition()
                               : QPointF(0.f, 0.f);

  info_label_->setText(QString(tr("Server Var: %1\n"
                                  "Room Var: %2\n"
                                  "Client Var: %3\n"
                                  "Ping: %4\n"
                                  "X: %5, \tY: %6\n"
                                  "Nickname: %7\n"))
                           .arg(controller_->GetServerVar())
                           .arg(controller_->GetRoomVar())
                           .arg(controller_->GetClientVar())
                           .arg(controller_->GetPing())
                           .arg(local_player_position.x())
                           .arg(local_player_position.y())
                           .arg(model_->GetLocalPlayerStats()->GetNickname()));
  info_label_->adjustSize();
}

void GameView::resizeEvent(QResizeEvent* event) {
  int width = event->size().width();
  int height = event->size().height();

  view_port_->resize(event->size());

  int height_of_bar = (height * 15) / 100;
  player_bar_->move(0, height - height_of_bar);
  player_bar_->resize(width, height_of_bar);
}