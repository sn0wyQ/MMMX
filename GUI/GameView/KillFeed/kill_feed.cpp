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

void KillFeed::AddKillNotification(const QString& killer_name,
                                   const QString& victim_name,
                                   WeaponType weapon_type) {
  QString message = killer_name + " killed " + victim_name;
  if (weapon_type != WeaponType::kNone) {
    auto weapon_name_str = Constants::GetStringFromEnumValue(weapon_type);
    weapon_name_str.remove(0, 1);
    message += " with " + weapon_name_str;
  }
  this->AddNotification(new KillFeedNotification(this, message));
}

void KillFeed::AddSpawnNotification(const QString& player_name) {
  this->AddNotification(
      new KillFeedNotification(this, player_name + " respawned"));
}

void KillFeed::paintEvent(QPaintEvent* paint_event) {
  QPainter painter(this);
  Constants::SetPainterHints(&painter);
  painter.translate(kLeftOffset, kDistanceBetweenNotifications);
  notifications_y_emulator_.MakeStepTo(0);
  painter.translate(0, std::round(notifications_y_emulator_.GetCurrentValue()));
  for (int i = static_cast<int>(notifications_.size()) - 1; i >= 0; i--) {
    notifications_[i]->Draw(&painter);
    painter.translate(0, kNotificationHeight + kDistanceBetweenNotifications);
  }
}

void KillFeed::resizeEvent(QResizeEvent* resize_event) {
  for (auto notification : notifications_) {
    notification->resize(this->width() - kLeftOffset - kRightOffset,
                         kNotificationHeight);
  }
  QWidget::resizeEvent(resize_event);
}

void KillFeed::AddNotification(KillFeedNotification* kill_feed_notification) {
  notifications_y_emulator_.SetCurrentValue(kOutOfScreenAnimationOffset);
  notifications_.push_back(kill_feed_notification);
  notifications_.back()->resize(this->width() - kLeftOffset - kRightOffset,
                                kNotificationHeight);
  if (notifications_.size() > kMaxNotificationsOnScreen) {
    notifications_[next_to_disappear_index_++]->Hide();
  }
}
