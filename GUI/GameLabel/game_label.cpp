#include "game_label.h"

GameLabel::GameLabel(QWidget* parent, GameDataModel* model)
  : QLabel(parent), model_(model) {
  converter_ = std::make_shared<Converter>(this);
}

void GameLabel::paintEvent(QPaintEvent* paint_event) {
  Painter painter(this,
                  converter_,
                  model_->IsLocalPlayerSet()
                  ? model_->GetLocalPlayer()->GetPosition()
                  : QPointF(0, 0));
  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->Draw(&painter);
  }
}

void GameLabel::resizeEvent(QResizeEvent* resize_event) {
  converter_->Update(static_cast<float>(resize_event->size().height()));
}
