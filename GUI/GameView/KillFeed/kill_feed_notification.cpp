#include <QPainter>
#include <QGraphicsOpacityEffect>
#include "kill_feed_notification.h"

#include "QDebug"

using Constants::KillFeedNotification::kOpacityAnimationStiffnessRatio;
using Constants::KillFeedNotification::kOpacityAnimationFrictionRatio;
using Constants::KillFeedNotification::kDisappearMsecs;
using Constants::KillFeedNotification::kBackgroundOpacityRatio;
using Constants::KillFeedNotification::kBackgroundOutlineWidth;
using Constants::KillFeedNotification::kRoundedRectXRadius;
using Constants::KillFeedNotification::kRoundedRectYRadius;

KillFeedNotification::KillFeedNotification(QWidget* parent,
                                           QString killer_name,
                                           QString killed_name,
                                           WeaponType weapon_type) :
    QWidget(parent),
    killer_name_(std::move(killer_name)),
    killed_name_(std::move(killed_name)),
    weapon_type_(weapon_type),
    opacity_emulator_(kOpacityAnimationStiffnessRatio,
                      kOpacityAnimationFrictionRatio) {
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetBounds(0.f, 255.f);
  disappear_timer_.start(kDisappearMsecs);
  disappear_timer_.setSingleShot(true);
  connect(&disappear_timer_, &QTimer::timeout,
          this, &KillFeedNotification::Disappear);
}

void KillFeedNotification::Draw(QPainter* painter) {
  painter->save();
  opacity_emulator_.MakeStepTo(opacity_target_);

  QColor pen_color(Qt::black);
  pen_color.setAlpha(static_cast<int>(opacity_emulator_.GetCurrentValue()));
  painter->setPen(QPen(pen_color));
  QColor brush_color(Qt::cyan);
  brush_color.setAlpha(static_cast<int>(kBackgroundOpacityRatio *
      opacity_emulator_.GetCurrentValue()));
  painter->setBrush(QBrush(brush_color));

  auto pen = painter->pen();
  pen.setWidthF(kBackgroundOutlineWidth);
  painter->setPen(pen);

  painter->drawRoundedRect(QRect(0, 0, this->width(), this->height()),
                           kRoundedRectXRadius, kRoundedRectYRadius);

  auto weapon_name = Constants::GetEnumValueToString(weapon_type_);
  weapon_name.remove(0, 1);
  QString message = killer_name_ + " killed " + killed_name_;
  if (weapon_type_ != WeaponType::kNull) {
    message += " with " + weapon_name;
  }
  painter->drawText(
      QRect(0, 0, this->width(), this->height()),
      Qt::AlignCenter, message);

  painter->restore();
}

void KillFeedNotification::Disappear() {
  opacity_target_ = 0;
}
