#include "game_view.h"

GameView::GameView(QWidget* parent, std::shared_ptr<ClientGameModel> model)
    : QWidget(parent),
      model_(std::move(model)),
      camera_motion_emulator_(Constants::kCameraStiffnessRatio,
                              Constants::kCameraFrictionRatio),
      fov_change_emulator_(Constants::kFovStiffnessRatio,
                           Constants::kFovFrictionRatio) {
  converter_ = std::make_shared<Converter>(this);
}

std::shared_ptr<Converter> GameView::GetConverter() {
  return converter_;
}

void GameView::paintEvent(QPaintEvent* paint_event) {
  // If LocalPlayer isn't set we don't want to draw anything
  if (!model_->IsLocalPlayerSet()) {
    was_player_set_ = false;
    return;
  }

  const auto& local_player = model_->GetLocalPlayer();
  auto player_pos = local_player->GetPosition();
  if (!was_player_set_) {
    was_player_set_ = true;
    camera_motion_emulator_.SetValue(QVector2D(local_player->GetPosition()));
    fov_change_emulator_.SetValue(local_player->GetFovRadius() / 1.2f);
  }

  camera_motion_emulator_.MakeStep(QVector2D(player_pos));
  auto local_center = camera_motion_emulator_.GetValue().toPointF();
  fov_change_emulator_.MakeStep(local_player->GetFovRadius());
  auto last_player_fov = fov_change_emulator_.GetValue();

  auto player_bar_offset =
      this->GetConverter()->ScaleFromScreenToGame(
          Constants::kPlayerBarHeightRatio * this->height() / 2.f);
  converter_->UpdateCoefficient(last_player_fov + player_bar_offset);
  Painter painter(this,
                  converter_,
                  local_center + QPointF(0, player_bar_offset));

  painter.setRenderHints(
      QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

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

  // Temporary FOV show
  painter.DrawEllipse(local_player->GetPosition(),
                      last_player_fov,
                      last_player_fov);
  painter.SetClipCircle(local_player->GetX(),
                        local_player->GetY(),
                        last_player_fov);

  std::vector<std::shared_ptr<GameObject>> filtered_objects
      = model_->GetFilteredByFovObjects();
  for (const auto& object : filtered_objects) {
    if (!object->IsMovable()) {
      object->Draw(&painter);
    }
  }
  for (const auto& object : filtered_objects) {
    if (object->IsMovable()) {
      object->Draw(&painter);
    }
  }

  for (const auto& object : model_->GetLocalBullets()) {
    object->Draw(&painter);
  }
}

void GameView::resizeEvent(QResizeEvent* resize_event) {
  converter_->UpdateCoefficient();
}

QPointF GameView::GetPlayerToCenterOffset() const {
  if (model_->IsLocalPlayerSet()) {
    auto player_bar_offset = converter_->ScaleFromScreenToGame(
        Constants::kPlayerBarHeightRatio * this->height() / 2.f);
    auto local_center = camera_motion_emulator_.GetValue().toPointF();
    return (local_center - model_->GetLocalPlayer()->GetPosition() +
        QPointF(0, player_bar_offset));
  }
  return QPoint();
}

void GameView::UpdateLocalCenter() {
}
