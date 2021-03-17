#include "converter.h"

Converter::Converter(QWidget* widget) : widget_(widget) {
  UpdateCoefficient();
  UpdateGameCenter(QPointF(Constants::kDefaultPlayerX,
                           Constants::kDefaultPlayerY));
}

QPointF Converter::GetCenter() const {
  return game_center_;
}

float Converter::GetCoef() const {
  return coefficient_;
}

void Converter::UpdateGameCenter(QPointF center) {
  game_center_ = center;
}

void Converter::UpdateCoefficient() {
  coefficient_ = static_cast<float>(widget_->height()) / Constants::kBaseViewHeight;
  screen_center_ = QPointF(static_cast<float>(widget_->width()) / 2.f,
                           static_cast<float>(widget_->height()) / 2.f);
}

QPointF Converter::PointFromGameToScreen(const QPointF& point) {
  QPointF delta = ScaleFromGameToScreen(point - game_center_);
  return screen_center_ - delta;
}

QPointF Converter::PointFromScreenToGame(const QPointF& point) {
  QPointF delta = ScaleFromScreenToGame(screen_center_ - point);
  return game_center_ - delta;
}
