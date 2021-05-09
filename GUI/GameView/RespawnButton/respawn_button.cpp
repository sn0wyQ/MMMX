#include "respawn_button.h"

using Constants::RespawnButton::kBackgroundColor;
using Constants::RespawnButton::kOutlineWidth;

using Constants::RespawnButton::kOpacityAnimationStiffnessRatio;
using Constants::RespawnButton::kTextFont;
using Constants::RespawnButton::kTextColor;
using Constants::RespawnButton::kInfillColor;
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

void RespawnButton::SetWaitValue(int64_t wait_secs) {
  wait_secs_ = wait_secs;
}

void RespawnButton::Hide() {
  opacity_target_ = 0.f;
}

void RespawnButton::paintEvent(QPaintEvent* event) {
  opacity_emulator_.MakeStepTo(opacity_target_);
  value_emulator_.MakeStepTo(total_holding_msecs_);
  QPainter painter(this);
  Constants::SetPainterHints(&painter);

  painter.setBrush(QBrush(kBackgroundColor));
  painter.setPen(QPen(Qt::black, kOutlineWidth));
  QRectF circle_rect(kOutlineWidth, kOutlineWidth,
                     this->width() - 2 * kOutlineWidth,
                     this->height() - 2 * kOutlineWidth);
  painter.drawEllipse(circle_rect);

  if (wait_secs_ < 1) {
    painter.setBrush(QBrush(kInfillColor));
    painter.setPen(QPen(Qt::transparent));
    circle_rect = QRect(kOutlineWidth * 1.5f, kOutlineWidth * 1.5f,
                        this->width() - 3 * kOutlineWidth,
                        this->height() - 3 * kOutlineWidth);
    auto angle = 360 * value_emulator_.GetCurrentValue()
        / (0.95f * Constants::kHoldingRespawnTime);
    painter.drawPie(circle_rect, 16 * 90, -16 * angle);

    QFont font(kTextFont);
    QString text = "Respawn";
    float factor =
        this->width() * 0.7f / painter.fontMetrics().horizontalAdvance(text);
    font.setPointSizeF(font.pointSizeF() * factor);
    painter.setFont(font);
    painter.setPen(QPen(kTextColor));
    painter.drawText(QRect(0, 0, this->width(), this->height()),
                     Qt::AlignCenter, text);
  } else {
    QFont font(kTextFont);
    QString text = QString::number(wait_secs_);
    float factor = this->height() * 0.9f / painter.fontMetrics().height();
    font.setPointSizeF(font.pointSizeF() * factor);
    painter.setFont(font);
    painter.setPen(QPen(kTextColor));
    painter.drawText(QRect(0, 0, this->width(), this->height()),
                     Qt::AlignCenter,
                     QString::number(wait_secs_));
  }

  opacity_effect_->setOpacity(opacity_emulator_.GetCurrentValue());
}
