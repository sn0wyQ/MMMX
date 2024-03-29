#include <cmath>
#include <utility>

#include <QGraphicsOpacityEffect>
#include <QPainter>

#include "kill_feed_notification.h"

using Constants::KillFeedNotification::kBackgroundOutlineColor;
using Constants::KillFeedNotification::kTextColor;
using Constants::KillFeedNotification::kTextFont;
using Constants::KillFeedNotification::kBackgroundColor;
using Constants::KillFeedNotification::kBackgroundOpacityMultiplier;

using Constants::KillFeedNotification::kOpacityAnimationStiffnessRatio;
using Constants::KillFeedNotification::kOpacityAnimationFrictionRatio;
using Constants::KillFeedNotification::kDisappearMsecs;
using Constants::KillFeedNotification::kBackgroundOpacityRatio;
using Constants::KillFeedNotification::kBackgroundOutlineWidth;
using Constants::KillFeedNotification::kRoundedRectXRadius;
using Constants::KillFeedNotification::kRoundedRectYRadius;

KillFeedNotification::KillFeedNotification(QWidget* parent, QString message) :
    QWidget(parent),
    message_(std::move(message)),
    opacity_emulator_(kOpacityAnimationStiffnessRatio,
                      kOpacityAnimationFrictionRatio) {
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetBounds(0.f, 255.f);
  disappear_timer_.start(kDisappearMsecs);
  disappear_timer_.setSingleShot(true);
  connect(&disappear_timer_, &QTimer::timeout,
          this, &KillFeedNotification::Hide);
}

void KillFeedNotification::Draw(QPainter* painter) {
  painter->save();
  opacity_emulator_.MakeStepTo(opacity_target_);

  painter->setBrush(QBrush(this->GetColorWithOpacity(kBackgroundColor,
         opacity_emulator_.GetCurrentValue() * kBackgroundOpacityMultiplier)));
  painter->setPen(QPen(this->GetColorWithOpacity(kBackgroundOutlineColor),
                       kBackgroundOutlineWidth));

  painter->drawRoundedRect(QRect(0, 0, this->width(), this->height()),
                           kRoundedRectXRadius, kRoundedRectYRadius);

  painter->setPen(QPen(this->GetColorWithOpacity(kTextColor)));

  painter->setFont(kTextFont);
  painter->drawText(
      QRect(0, 0, this->width(), this->height()),
      Qt::AlignCenter, message_);

  painter->restore();
}

void KillFeedNotification::Hide() {
  opacity_target_ = 0;
}

QColor KillFeedNotification::GetColorWithOpacity(const QColor& color,
                                                 int opacity) {
  if (opacity == -1) {
    opacity = static_cast<int>(std::round(opacity_emulator_.GetCurrentValue()));
  }
  auto result = color;
  result.setAlpha(opacity);
  return result;
}

QString KillFeedNotification::GetMessage() const {
  return message_;
}
