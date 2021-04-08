#include "game_view.h"

GameView::GameView(QWidget* parent, std::shared_ptr<ClientGameModel> model)
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
                  : QPointF(0.f, 0.f));

  std::vector<std::shared_ptr<GameObject>> not_filtered_objects
    = model_->GetNotFilteredByFovObjects();
  for (const auto& object : not_filtered_objects) {
    if (!object->IsMovable()) {
      object->Draw(&painter);
    }
  }
  for (const auto& object : not_filtered_objects) {
    if (object->IsMovable()) {
      object->Draw(&painter);
    }
  }

  // If LocalPlayer isn't set we don't want to draw anything non-constant
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  // Temporary FOV show
  const auto& local_player = model_->GetLocalPlayer();
  painter.DrawEllipse(local_player->GetPosition(),
                      local_player->GetFovRadius(),
                      local_player->GetFovRadius());
  painter.SetClipCircle(local_player->GetX(),
                        local_player->GetY(),
                        local_player->GetFovRadius());

  std::vector<std::shared_ptr<GameObject>> filtered_objects
      = model_->GetFilteredByFovObjects();
  for (const auto& object : filtered_objects) {
    if (object->IsInFov() && !object->IsMovable()) {
      object->Draw(&painter);
    }
  }
  for (const auto& object : filtered_objects) {
    if (object->IsInFov() && object->IsMovable()) {
      object->Draw(&painter);
    }
  }
}

void GameView::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoefficient();
}
