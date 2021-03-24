#include "game_view.h"

#include <utility>

GameView::GameView(QWidget* parent, std::shared_ptr<GameDataModel> model)
  : QWidget(parent), model_(std::move(model)) {
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
  std::vector<std::shared_ptr<Box>> boxes = model_->GetBoxes();
  for (const auto& box : boxes) {
    box->Draw(&painter);
  }
  std::vector<std::shared_ptr<Tree>> trees = model_->GetTrees();
  for (const auto& tree : trees) {
    tree->Draw(&painter);
  }

  std::vector<std::shared_ptr<Player>> players = model_->GetPlayers();
  for (const auto& player : players) {
    player->Draw(&painter);
  }
}

void GameView::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoefficient();
}
