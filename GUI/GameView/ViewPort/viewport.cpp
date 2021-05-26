#include "viewport.h"

ViewPort::ViewPort(QWidget* parent, ClientController* controller)
  : QOpenGLWidget(parent),
    controller_(controller),
    model_(controller_->GetModel()),
    converter_(std::make_shared<Converter>(this)),
    camera_motion_emulator_(Constants::kCameraStiffnessRatio,
                            Constants::kCameraFrictionRatio),
    fov_change_emulator_(Constants::kFovStiffnessRatio,
                         Constants::kFovFrictionRatio) {
  this->PrepareFovImage();
}

std::shared_ptr<Converter> ViewPort::GetConverter() {
  return converter_;
}

void ViewPort::mouseMoveEvent(QMouseEvent* mouse_event) {
  controller_->MouseMoveEvent(mouse_event);
}

void ViewPort::Update() {
  // If LocalPlayer isn't set we don't want to draw anything
  if (!model_->IsLocalPlayerSet()) {
    was_player_set_ = false;
    return;
  }

  Painter painter(this, this->GetConverter());
  painter.save();
  painter.setBrush(QBrush(Qt::white));
  painter.drawRect(0, 0, this->width(), this->height());
  painter.setBrush(QBrush(Qt::transparent));
  Constants::SetPainterHints(&painter);

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
  painter.translate(translation.toPoint());

  auto view_rect_offset =
      QPointF(this->width(), this->height()) / 2.f;
  view_rect_offset = converter_->ScaleFromScreenToGame(view_rect_offset);
  view_rect_offset.setY(view_rect_offset.y() + 1.2f * player_bar_offset);
  auto view_rect = QRectF(local_center - view_rect_offset,
                          local_center + view_rect_offset);

  std::vector<std::shared_ptr<GameObject>> drawn_objects;
  this->DrawObjects(model_->GetNotFilteredByFovObjects(), view_rect,
                    &drawn_objects,
                    &painter);

  view_rect_offset = QPointF(this->height(), this->height()) / 2.f;
  view_rect_offset = converter_->ScaleFromScreenToGame(view_rect_offset);
  view_rect = QRectF(local_center - view_rect_offset,
                     local_center + view_rect_offset);

  QPixmap pixmap_for_filtered(this->size());
  pixmap_for_filtered.fill(Qt::transparent);
  Painter painter_for_filtered(&pixmap_for_filtered, this->converter_);
  painter_for_filtered.setClipRect(this->rect());
  Constants::SetPainterHints(&painter_for_filtered);
  painter_for_filtered.translate(translation.toPoint());
  if (!Settings::GetInstance().GetValueByKey<bool>("main/smooth_fov")) {
    painter_for_filtered.SetClipCircle(local_player->GetX(),
                                       local_player->GetY(),
                                       last_player_fov);
  }

  this->DrawObjects(model_->GetFilteredByFovObjects(), view_rect,
                    &drawn_objects,
                    &painter_for_filtered);

  for (const auto& object : model_->GetLocalBullets()) {
    if (view_rect.intersects(object->GetBoundingRect())) {
      object->Draw(&painter_for_filtered);
    }
  }

  for (const auto& object : drawn_objects) {
    DrawBars(object, &painter_for_filtered);
  }

  painter.restore();

  if (Settings::GetInstance().GetValueByKey<bool>("main/smooth_fov")) {
    auto filtered_image = pixmap_for_filtered.toImage();
    filtered_image.setAlphaChannel(fov_for_objects_);
    painter.drawImage(this->rect(), filtered_image);
  } else {
    painter.drawPixmap(this->rect(), pixmap_for_filtered);
  }
  painter.setClipping(false);
  painter.setOpacity(Constants::kCloudsOpacity);
  painter.drawPixmap(this->rect(), fov_image_);

  need_to_update_ = true;
  this->update();
}

void ViewPort::paintEvent(QPaintEvent* paint_event) {
  if (!need_to_update_) {
    return;
  }
  need_to_update_ = false;
}

void ViewPort::resizeGL(int w, int h) {
  converter_->UpdateCoefficient();
  this->PrepareFovImage();
}

QPointF ViewPort::GetPlayerToCenterOffset() const {
  if (model_->IsLocalPlayerSet()) {
    auto player_bar_offset = converter_->ScaleFromScreenToGame(
        Constants::kPlayerBarHeightRatio * this->height() / 2.f);
    auto local_center = camera_motion_emulator_.GetCurrentValue().toPointF();
    return (local_center - model_->GetLocalPlayer()->GetPosition() +
        QPointF(0, player_bar_offset));
  }
  return QPoint();
}

void ViewPort::DrawObjects(
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

void ViewPort::DrawBars(const std::shared_ptr<GameObject>& object,
                        Painter* painter) {
  painter->save();
  painter->Translate(object->GetPosition());
  if (object->GetId() != model_->GetLocalPlayer()->GetId()) {
    object->DrawHealthBar(painter);
    object->DrawLevel(painter);
    if (object->GetType() == GameObjectType::kPlayer) {
      QString nickname =
          model_->GetPlayerStatsByPlayerId(object->GetId())->GetNickname();
      object->DrawNickname(painter, nickname);
    }
  }
  painter->restore();
}

void ViewPort::PrepareFovImage() {
  QImage clouds(":clouds.png");
  clouds = clouds.scaled(this->size(),
                         Qt::AspectRatioMode::IgnoreAspectRatio,
                         Qt::TransformationMode::SmoothTransformation);
  QImage fov(":fov.png");
  QImage fov_inverted(":fov_inverted.png");
  auto player_bar_offset =
          Constants::kPlayerBarHeightRatio * this->height() / 2.f;
  auto fov_height = this->height() - player_bar_offset;

  QPixmap fov_pixmap(clouds.size());
  fov_pixmap.fill(Qt::white);
  QPainter fov_painter(&fov_pixmap);

  QPixmap fov_inverted_pixmap(clouds.size());
  fov_inverted_pixmap.fill(Qt::black);
  QPainter fov_inverted_painter(&fov_inverted_pixmap);

  fov_painter.drawImage(QRectF((this->width() - fov_height) / 2.f, 0,
                               fov_height, fov_height), fov);

  fov_inverted_painter.drawImage(QRectF((this->width() - fov_height) / 2.f, 0,
                                   fov_height, fov_height), fov_inverted);

  fov_for_objects_ = fov_inverted_pixmap.toImage();

  clouds.setAlphaChannel(fov_pixmap.toImage());
  fov_image_.convertFromImage(clouds.scaled(this->size()));
}
