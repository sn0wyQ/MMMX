#ifndef GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_
#define GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_

#include <QWidget>
#include <QTimer>

#include "SpringEmulator/spring_emulator.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

namespace Constants::KillFeedNotification {

const float kOpacityAnimationStiffnessRatio = 0.003f;
const float kOpacityAnimationFrictionRatio = 0.08f;
const int kDisappearMsecs = 5000;
const float kBackgroundOpacityRatio = 0.5f;
const float kBackgroundOutlineWidth = 2.f;
const float kRoundedRectXRadius = 5.f;
const float kRoundedRectYRadius = 5.f;

}  // namespace Constants::KillFeedNotification

class KillFeedNotification : public QWidget {
  Q_OBJECT

 public:
  KillFeedNotification(QWidget* parent,
                       QString killer_name,
                       QString killed_name,
                       WeaponType weapon_type);

  void Disappear();

  void Draw(QPainter* painter);

 private:
  QString killer_name_;
  QString killed_name_;
  WeaponType weapon_type_;
  SpringEmulator<float, true> opacity_emulator_;
  float opacity_target_{255};
  QTimer disappear_timer_;
};

#endif  // GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_
