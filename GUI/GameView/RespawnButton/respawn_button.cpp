#include "respawn_button.h"

using Constants::RespawnButton::kOpacityAnimationStiffnessRatio;
using Constants::RespawnButton::kOpacityAnimationFrictionRatio;
using Constants::RespawnButton::kValueAnimationStiffnessRatio;
using Constants::RespawnButton::kValueAnimationFrictionRatio;

RespawnButton::RespawnButton(QWidget* parent) :
    QWidget(parent),
    opacity_effect_(new QGraphicsOpacityEffect(this)),
    opacity_emulator_(kOpacityAnimationStiffnessRatio,
                      kOpacityAnimationFrictionRatio),
    value_emulator_(kValueAnimationStiffnessRatio,
                    kValueAnimationFrictionRatio) {
  value_emulator_.SetBounds(0, Constants::kHoldingRespawnTime);
  value_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetBounds(0.01, 1);
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_target_ = 0.f;
  this->setGraphicsEffect(opacity_effect_);
}

void RespawnButton::Show() {
  opacity_target_ = 1.f;
}

void RespawnButton::SetValue(int64_t total_holding_msecs) {
  total_holding_msecs_ = total_holding_msecs;
}

void RespawnButton::Hide() {
  opacity_target_ = 0.f;
}

void RespawnButton::Resize(const QSize& size) {}

void RespawnButton::paintEvent(QPaintEvent* event) {
  opacity_emulator_.MakeStepTo(opacity_target_);
  value_emulator_.MakeStepTo(total_holding_msecs_);
  QPainter painter(this);
  painter.setRenderHints(
      QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  painter.setBrush(QBrush(Qt::transparent));
  painter.drawEllipse(0, 0, this->width(), this->height());
  painter.setBrush(QBrush(Qt::cyan));
  painter.setPen(QPen(Qt::transparent));
  qInfo() << total_holding_msecs_;
  painter.drawPie(QRect(0, 0, this->width(), this->height()), 16 * 90,
                  -16 * 360 * value_emulator_.GetCurrentValue() /
                      Constants::kHoldingRespawnTime);

  painter.setPen(QPen(Qt::black));
  painter.drawText(QRect(0, 0, this->width(), this->height()),
                   Qt::AlignCenter,
                   "Respawn");

  opacity_effect_->setOpacity(opacity_emulator_.GetCurrentValue());
}
