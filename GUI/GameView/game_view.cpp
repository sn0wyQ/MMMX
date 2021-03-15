#include "game_view.h"

GameView::GameView(QWidget* parent, GameDataModel* model)
  : QWidget(parent), model_(model) {
  converter_ = std::make_shared<Converter>(this);
}

std::shared_ptr<Converter> GameView::GetConverter() {
  return converter_;
}

void GameView::paintEvent(QPaintEvent* paint_event) {
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

void GameView::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoef();
}
