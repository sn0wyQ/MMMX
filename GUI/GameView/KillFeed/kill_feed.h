#ifndef GUI_GAMEVIEW_KILLFEED_KILL_FEED_H_
#define GUI_GAMEVIEW_KILLFEED_KILL_FEED_H_

#include <deque>

#include <QWidget>

#include "GUI/GameView/KillFeed/kill_feed_notification.h"
#include "GUI/Animations/spring_emulator.h"

namespace Constants::KillFeed {

const float kOutOfScreenAnimationOffset = 40.f;
const int kNotificationHeight = 30.f;
const int kLeftOffset = 5.f;
const int kRightOffset = 5.f;
const int kMaxNotificationsOnScreen = 5;
const float kDistanceBetweenNotifications = 5.f;
const float kAnimationStiffnessRatio = 0.05f;
const float kAnimationFrictionRatio = 0.1f;

}  // namespace Constants::KillFeed

class KillFeed : public QWidget {
  Q_OBJECT

 public:
  explicit KillFeed(QWidget* parent);

  void AddKillNotification(const QString& killer_name,
                           const QString& victim_name,
                           WeaponType weapon_type);
  void AddSpawnNotification(const QString& player_name);
  void AddGameStartedNotification();

  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

 private:
  void AddNotification(KillFeedNotification* kill_feed_notification);

  SpringEmulator<float, false> notifications_y_emulator_;
  std::deque<KillFeedNotification*> notifications_;
  int next_to_disappear_index_{0};
};

#endif  // GUI_GAMEVIEW_KILLFEED_KILL_FEED_H_
