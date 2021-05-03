#include "converter.h"

Converter::Converter(QWidget* widget) : widget_(widget) {
  UpdateCoefficient();
}

QPointF Converter::GetGameCenter() const {
  return game_center_;
}

float Converter::GetCoefficient() const {
  return coefficient_;
}

void Converter::UpdateGameCenter(QPointF game_center) {
  game_center_ = game_center;
}

void Converter::UpdateCoefficient(float fov) {
  coefficient_ =
      static_cast<float>(widget_->height()) / Constants::kBaseViewHeight;
  coefficient_ *= (Constants::kDefaultEntityFov / fov);
  coefficient_ /=
      ((Constants::kDefaultEntityFov * 1.9f) / Constants::kBaseViewHeight);
  screen_center_ = QPointF(static_cast<float>(widget_->width()) / 2.f,
                           static_cast<float>(widget_->height()) / 2.f);
}

QPointF Converter::PointFromGameToScreen(const QPointF& point) const {
  QPointF delta = ScaleFromGameToScreen(point - game_center_);
  return screen_center_ - delta;
}

QPointF Converter::PointFromScreenToGame(const QPointF& point) const {
  QPointF delta = ScaleFromScreenToGame(screen_center_ - point);
  return game_center_ - delta;
}
