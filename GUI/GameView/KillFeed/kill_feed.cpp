#include <cmath>
#include <utility>

#include <QPainter>

#include "kill_feed.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

using Constants::KillFeed::kOutOfScreenAnimationOffset;
using Constants::KillFeed::kNotificationHeight;
using Constants::KillFeed::kLeftOffset;
using Constants::KillFeed::kRightOffset;
using Constants::KillFeed::kMaxNotificationsOnScreen;
using Constants::KillFeed::kDistanceBetweenNotifications;
using Constants::KillFeed::kAnimationStiffnessRatio;
using Constants::KillFeed::kAnimationFrictionRatio;

KillFeed::KillFeed(QWidget* parent) :
    QWidget(parent),
    notifications_y_emulator_(kAnimationStiffnessRatio,
                              kAnimationFrictionRatio) {}

void KillFeed::AddNotification(QString killer_name,
                               QString killed_name,
                               WeaponType weapon_type) {
  notifications_y_emulator_.SetCurrentValue(kOutOfScreenAnimationOffset);
  notifications_.push_back(
      new KillFeedNotification(this,
                               std::move(killer_name),
                               std::move(killed_name),
                               weapon_type));
  notifications_.back()->resize(this->width() - kLeftOffset - kRightOffset,
                                kNotificationHeight);
  if (notifications_.size() > kMaxNotificationsOnScreen) {
    notifications_[next_to_disappear_index_++]->Disappear();
  }
}

void KillFeed::paintEvent(QPaintEvent* paint_event) {
  QPainter painter(this);
  painter.setRenderHints(
      QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.save();
  painter.translate(kLeftOffset, kDistanceBetweenNotifications);
  notifications_y_emulator_.MakeStepTo(0);
  painter.translate(0, std::round(notifications_y_emulator_.GetCurrentValue()));
  for (int i = static_cast<int>(notifications_.size()) - 1; i >= 0; i--) {
    notifications_[i]->Draw(&painter);
    painter.translate(0, kNotificationHeight + kDistanceBetweenNotifications);
  }
  painter.restore();
}

void KillFeed::resizeEvent(QResizeEvent* resize_event) {
  for (auto notification : notifications_) {
    notification->resize(this->width() - kLeftOffset - kRightOffset,
                         kNotificationHeight);
  }
  QWidget::resizeEvent(resize_event);
}
