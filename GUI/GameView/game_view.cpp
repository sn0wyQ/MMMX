#include "game_view.h"

GameView::GameView(QWidget* parent, std::shared_ptr<ClientGameModel> model)
  : QWidget(parent), model_(std::move(model)) {
  converter_ = std::make_shared<Converter>(this);
}

std::shared_ptr<Converter> GameView::GetConverter() {
  return converter_;
}

void GameView::paintEvent(QPaintEvent* paint_event) {
  this->UpdateLocalCenter();
  auto player_bar_offset =
      this->GetConverter()->ScaleFromScreenToGame(
          Constants::kPlayerBarHeightRatio * this->height() / 2.f);
  Painter painter(this,
                  converter_,
                  local_center_ + QPointF(0, player_bar_offset));

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

  // If LocalPlayer isn't set we don't want to draw anything non-constant
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  converter_->UpdateCoefficient(
      model_->GetLocalPlayer()->GetFovRadius() + player_bar_offset);

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
    return (local_center_ - model_->GetLocalPlayer()->GetPosition() +
        QPointF(0, player_bar_offset));
  }
  return QPoint();
}

void GameView::UpdateLocalCenter() {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto player_pos = model_->GetLocalPlayer()->GetPosition();
  auto time = QDateTime::currentMSecsSinceEpoch();
  if (last_paint_event_time_ == -1) {
    local_center_ = player_pos;
    last_paint_event_time_ = time;
  }

  // Lets divide dt by 10 so the constants wont be too small
  float delta_time = static_cast<float>(time - last_paint_event_time_) / 10.f;
  last_paint_event_time_ = time;
  QVector2D camera_dot(local_center_);
  QVector2D player_dot(player_pos);

  // By the Hooke's Law: F = -k * x where x = |A - B| - l_0.
  // Owr "spring's" length is 0, so l_0 = 0
  QVector2D f = -Constants::kCameraStiffness * (camera_dot - player_dot);
  // v = F*dt/m (let m = 1)
  camera_velocity_ += f * delta_time;
  camera_velocity_ -=
      camera_velocity_ * Constants::kCameraFrictionForce * delta_time;
  local_center_ += (camera_velocity_ * delta_time).toPointF();
}
