#include "game_view.h"

GameView::GameView(QWidget* parent, std::shared_ptr<ClientGameModel> model)
    : QOpenGLWidget(parent),
      model_(std::move(model)),
      converter_(std::make_shared<Converter>(this)),
      camera_motion_emulator_(Constants::kCameraStiffnessRatio,
                              Constants::kCameraFrictionRatio),
      fov_change_emulator_(Constants::kFovStiffnessRatio,
                           Constants::kFovFrictionRatio) {}

std::shared_ptr<Converter> GameView::GetConverter() {
  return converter_;
}

void GameView::Update() {
  Painter painter(this, this->GetConverter());
  painter.setBrush(QBrush(Qt::white));
  painter.drawRect(0, 0, this->width(), this->height());
  painter.setBrush(QBrush(Qt::transparent));
  Constants::SetPainterHints(&painter);

  // If LocalPlayer isn't set we don't want to draw anything
  if (!model_->IsLocalPlayerSet()) {
    was_player_set_ = false;
    return;
  }

  const auto& local_player = model_->GetLocalPlayer();
  auto player_pos = local_player->GetPosition();
  if (!was_player_set_) {
    was_player_set_ = true;
    camera_motion_emulator_.SetCurrentValue(
        QVector2D(local_player->GetPosition()));
    fov_change_emulator_.SetCurrentValue(local_player->GetFovRadius() / 1.2f);
  }

  camera_motion_emulator_.MakeStepTo(QVector2D(player_pos));
  auto local_center = camera_motion_emulator_.GetCurrentValue().toPointF();
  fov_change_emulator_.MakeStepTo(local_player->GetFovRadius());
  auto last_player_fov = fov_change_emulator_.GetCurrentValue();

  auto player_bar_offset =
      this->GetConverter()->ScaleFromScreenToGame(
          Constants::kPlayerBarHeightRatio * this->height() / 2.f);
  converter_->UpdateCoefficient(last_player_fov + player_bar_offset);

  // Setting screen centre to Player's position
  auto translation = QPointF(this->width(), this->height()) / 2.f
      - converter_->ScaleFromGameToScreen(
          local_center + QPointF(0, player_bar_offset));
  painter.translate(translation);

  auto view_rect_offset =
      QPointF(this->width(), this->height()) / 2.f;
  view_rect_offset = converter_->ScaleFromScreenToGame(view_rect_offset);
  auto view_rect = QRectF(local_center - view_rect_offset,
                          local_center + view_rect_offset);

  std::vector<std::shared_ptr<GameObject>> drawn_objects;
  this->DrawObjects(model_->GetNotFilteredByFovObjects(), view_rect,
                    &drawn_objects,
                    &painter);

  // Temporary FOV show
  view_rect_offset = QPointF(this->height(), this->height()) / 2.f;
  view_rect_offset = converter_->ScaleFromScreenToGame(view_rect_offset);
  view_rect = QRectF(local_center - view_rect_offset,
                     local_center + view_rect_offset);
  painter.DrawEllipse(local_player->GetPosition(),
                        last_player_fov,
                        last_player_fov);
  painter.SetClipCircle(local_player->GetX(),
                          local_player->GetY(),
                          last_player_fov);

  this->DrawObjects(model_->GetFilteredByFovObjects(), view_rect,
                    &drawn_objects,
                    &painter);

  for (const auto& object : model_->GetLocalBullets()) {
    if (view_rect.intersects(object->GetBoundingRect())) {
      object->Draw(&painter);
    }
  }

  for (const auto& object : drawn_objects) {
    DrawBars(object, &painter);
  }
  need_to_update_ = true;
  this->update();
}

void GameView::paintEvent(QPaintEvent* paint_event) {
  if (!need_to_update_) {
    return;
  }
  need_to_update_ = false;
}

void GameView::resizeGL(int w, int h) {
  converter_->UpdateCoefficient();
}

QPointF GameView::GetPlayerToCenterOffset() const {
  if (model_->IsLocalPlayerSet()) {
    auto player_bar_offset = converter_->ScaleFromScreenToGame(
        Constants::kPlayerBarHeightRatio * this->height() / 2.f);
    auto local_center = camera_motion_emulator_.GetCurrentValue().toPointF();
    return (local_center - model_->GetLocalPlayer()->GetPosition() +
        QPointF(0, player_bar_offset));
  }
  return QPoint();
}

void GameView::DrawObjects(
    const std::vector<std::shared_ptr<GameObject>>& objects,
    const QRectF& view_rect,
    std::vector<std::shared_ptr<GameObject>>* drawn_objects,
    Painter* painter) {
  for (const auto& object : objects) {
    if (!object->IsMovable()) {
      if (view_rect.intersects(object->GetBoundingRect())) {
        if (object->IsNeedToDraw()) {
          object->Draw(painter);
          drawn_objects->emplace_back(object);
        }
      }
    }
  }
  for (const auto& object : objects) {
    if (object->IsMovable()) {
      if (view_rect.intersects(object->GetBoundingRect())) {
        if (object->IsNeedToDraw()) {
          object->Draw(painter);
          drawn_objects->emplace_back(object);
        }
      }
    }
  }
}

void GameView::DrawBars(const std::shared_ptr<GameObject>& object,
                        Painter* painter) {
  painter->save();
  painter->Translate(object->GetPosition());
  object->DrawHealthBar(painter);
  object->DrawLevel(painter);
  if (object->GetType() == GameObjectType::kPlayer) {
    QString nickname =
        model_->GetPlayerStatsByPlayerId(object->GetId())->GetNickname();
    object->DrawNickname(painter, nickname);
  }
  painter->restore();
}
