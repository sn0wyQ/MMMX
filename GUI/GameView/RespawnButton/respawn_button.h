#ifndef GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_
#define GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QWidget>

#include "constants.h"
#include "SpringEmulator/spring_emulator.h"

namespace Constants::RespawnButton {

const QColor kTextColor(Qt::black);
const QFont kTextFont("Roboto Mono");
const QColor kInfillColor(QColor(0, 255, 109, 200));
const QColor kBackgroundColor(QColor(0, 255, 109, 120));

const float kOpacityAnimationStiffnessRatio = 0.003f;
const float kOpacityAnimationFrictionRatio = 0.08f;
const float kValueAnimationStiffnessRatio = 0.08f;
const float kValueAnimationFrictionRatio = 0.5f;
const float kOutlineWidth = 2.f;

}  // namespace Constants::RespawnButton

class RespawnButton : public QWidget {
  Q_OBJECT

 public:
  explicit RespawnButton(QWidget* parent);

  void Show();
  void SetValue(int64_t total_holding_msecs);
  void SetWaitValue(int64_t wait_secs);
  void Hide();

  void paintEvent(QPaintEvent* event) override;

 private:
  QGraphicsOpacityEffect* opacity_effect_;
  int64_t total_holding_msecs_{};
  int64_t wait_secs_{};
  SpringEmulator<float, true> opacity_emulator_;
  SpringEmulator<float, true> value_emulator_;
  float opacity_target_;
};


#endif  // GUI_GAMEVIEW_RESPAWNBUTTON_RESPAWN_BUTTON_H_
