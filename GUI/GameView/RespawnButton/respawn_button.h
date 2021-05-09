#ifndef GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_
#define GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QWidget>

#include "constants.h"
#include "SpringEmulator/spring_emulator.h"

namespace Constants::RespawnButton {

const QColor kTextColor(Qt::black);
const QColor kBackgroundOutlineColor(Qt::black);
const QColor kBackgroundColor(Qt::cyan);
const float kBackgroundOpacityMultiplier = 0.5f;

const float kOpacityAnimationStiffnessRatio = 0.003f;
const float kOpacityAnimationFrictionRatio = 0.08f;
const float kValueAnimationStiffnessRatio = 0.08f;
const float kValueAnimationFrictionRatio = 0.3f;
const int kDisappearMsecs = 5000;
const float kBackgroundOpacityRatio = 0.5f;
const float kBackgroundOutlineWidth = 2.f;
const float kRoundedRectXRadius = 5.f;
const float kRoundedRectYRadius = 5.f;

}  // namespace Constants::RespawnButton

class RespawnButton : public QWidget {
 Q_OBJECT

 public:
  RespawnButton(QWidget* parent);

  void Show();
  void SetValue(int64_t total_holding_msecs);
  void Hide();

  void Resize(const QSize& size);
  void paintEvent(QPaintEvent* event) override;

 private:
  QGraphicsOpacityEffect* opacity_effect_;
  int64_t total_holding_msecs_{};
  SpringEmulator<float, true> opacity_emulator_;
  SpringEmulator<float, true> value_emulator_;
  float opacity_target_;
};


#endif  // GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_
