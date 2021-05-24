#ifndef GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_
#define GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_

#include <QTimer>
#include <QWidget>

#include "GUI/Animations/spring_emulator.h"
#include "Weapon/WeaponSettings/weapon_settings.h"

namespace Constants::KillFeedNotification {

const QColor kTextColor(Qt::black);
const QFont kTextFont("Roboto Mono");
const QColor kBackgroundOutlineColor(Qt::black);
const QColor kBackgroundColor(Qt::cyan);
const float kBackgroundOpacityMultiplier = 0.5f;

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
  KillFeedNotification(QWidget* parent, QString message);

  void Hide();
  void Draw(QPainter* painter);
  QColor GetColorWithOpacity(const QColor& color,
                             int opacity = -1);
  QString GetMessage() const;

 private:
  QString message_;
  SpringEmulator<float, true> opacity_emulator_;
  float opacity_target_{255};
  QTimer disappear_timer_;
};

#endif  // GUI_GAMEVIEW_KILLFEED_KILL_FEED_NOTIFICATION_H_
