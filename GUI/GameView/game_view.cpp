#include "game_view.h"

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

  // ----------------- CONSTANT OBJECTS -----------------

  std::vector<std::shared_ptr<GameObject>> constant_objects
    = model_->GetConstantObjects();
  for (const auto& object : constant_objects) {
    if (object->IsInFov()) {
      object->Draw(&painter);
    }
  }

  // ------------------ DYNAMIC OBJECTS -----------------

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

  std::vector<std::shared_ptr<MovableObject>> movable_objects
    = model_->GetMovableObjects();
  for (const auto& object : movable_objects) {
    if (object->IsInFov()) {
      object->Draw(&painter);
    }
  }
}

void GameView::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoefficient();
}
