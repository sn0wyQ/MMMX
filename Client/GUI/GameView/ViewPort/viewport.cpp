#include "viewport.h"

ViewPort::ViewPort(QWidget* parent, std::shared_ptr<ClientGameModel> model)
  : QWidget(parent), model_(std::move(model)) {
  this->setMouseTracking(true);

  converter_ = std::make_shared<Converter>(this);
}

std::shared_ptr<Converter> ViewPort::GetConverter() {
  return converter_;
}

void ViewPort::paintEvent(QPaintEvent* paint_event) {
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

  converter_->UpdateCoefficient(model_->GetLocalPlayer()->GetFovRadius());

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

  for (const auto& object : model_->GetLocalBullets()) {
    if (object->IsInFov()) {
      object->Draw(&painter);
    }
  }
}

void ViewPort::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoefficient();
}
