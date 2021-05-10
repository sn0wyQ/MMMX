#include <QBitmap>
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
using Constants::RespawnButton::kPositionAnimationStiffnessRatio;
using Constants::RespawnButton::kPositionAnimationFrictionRatio;
using Constants::RespawnButton::kSizeAnimationSpeed;

RespawnButton::RespawnButton(QWidget* parent) :
    QWidget(parent),
    opacity_effect_(new QGraphicsOpacityEffect(this)),
    opacity_emulator_(kOpacityAnimationStiffnessRatio,
                      kOpacityAnimationFrictionRatio),
    value_emulator_(kValueAnimationStiffnessRatio,
                    kValueAnimationFrictionRatio),
    position_emulator_(kPositionAnimationStiffnessRatio,
                       kPositionAnimationFrictionRatio),
    size_ratio_emulator_(kSizeAnimationSpeed) {
  value_emulator_.SetBounds(0, Constants::kHoldingRespawnTime);
  size_ratio_emulator_.SetPath(0.9, 1);
  size_ratio_emulator_.SetCurrentValue(1);
  value_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetBounds(0.01, 1);
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_target_ = 0.f;
  position_target_ = QPointF();
  position_emulator_.SetCurrentValue(position_target_);
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
  position_emulator_.MakeStepTo(position_target_);
  this->move(position_emulator_.GetCurrentValue().toPoint());
  opacity_emulator_.MakeStepTo(opacity_target_);
  value_emulator_.MakeStepTo(total_holding_msecs_);
  QPainter painter(this);
  Constants::SetPainterHints(&painter);

  QPixmap gray_heart(":RespawnHeartGray.png");
  QPixmap rgb_heart(":RespawnHeart.png");
  float size = size_ratio_emulator_.GetCurrentValue() *
      std::max(this->width(), this->height()) / 1.f;
  QRectF draw_rect((this->width() - size) / 2.f,
                   (this->height() - size) / 2.f,
                   size, size);
  painter.drawPixmap(draw_rect, gray_heart, QRectF(0, 0, gray_heart.width(),
                                                   gray_heart.height()));
  QBitmap mask(QSize(rgb_heart.size()));
  mask.fill();
  QPainter mask_painter(&mask);

  if (wait_secs_ <= 0) {
    auto angle = 360 * value_emulator_.GetCurrentValue()
        / (0.95f * Constants::kHoldingRespawnTime);
    mask_painter.setBrush(QBrush(Qt::black));
    if (angle > 2.f) {
      mask_painter.drawPie(0, 0, mask.width(), mask.height(),
                           16 * 90, -16 * angle);
    }
    rgb_heart.setMask(mask);
    painter.drawPixmap(draw_rect, rgb_heart, QRectF(0, 0, rgb_heart.width(),
                                                    rgb_heart.height()));

    QFont font(kTextFont);
    QString text = "Respawn";
    float factor =
        this->width() / painter.fontMetrics().horizontalAdvance(text);
    font.setPointSizeF(0.6 * font.pointSizeF() * factor);
    painter.setFont(font);
    painter.setPen(QPen(kTextColor));
    painter.drawText(QRectF(draw_rect.x(),
                            draw_rect.y() - 12,
                            draw_rect.width(),
                            draw_rect.height()), Qt::AlignCenter, text);
  } else {
    QFont font(kTextFont);
    QString text = QString::number(wait_secs_);
    float factor_h = this->height() / painter.fontMetrics().height();
    float factor_w = this->width() /
        painter.fontMetrics().horizontalAdvance(text);
    font.setPointSizeF(font.pointSizeF() * 0.3f * std::min(factor_h, factor_w));
    painter.setFont(font);
    painter.setPen(QPen(kTextColor));
    painter.drawText(draw_rect, Qt::AlignCenter, text);
  }

  opacity_effect_->setOpacity(opacity_emulator_.GetCurrentValue());
}

void RespawnButton::Move(const QPointF& point) {
  position_target_ = point;
}

void RespawnButton::Resize(const QSize& size) {
  real_size_ = size;
  this->resize(size);
}
