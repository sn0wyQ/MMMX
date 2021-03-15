#include "game_label.h"

GameLabel::GameLabel(QWidget* parent, GameDataModel* model)
  : QLabel(parent), model_(model) {
  converter_ = std::make_shared<Converter>(this);
}

std::shared_ptr<Converter> GameLabel::GetConverter() {
  return converter_;
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
  converter_->UpdateCoef(static_cast<float>(resize_event->size().height()));
}
