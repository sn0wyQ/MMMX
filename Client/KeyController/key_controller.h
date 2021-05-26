#ifndef CLIENT_KEYCONTROLLER_KEY_CONTROLLER_H_
#define CLIENT_KEYCONTROLLER_KEY_CONTROLLER_H_

#include <map>
#include <vector>
#include <unordered_map>

#include <QDateTime>
#include <QSettings>
#include <QGraphicsOpacityEffect>
#include <QStandardPaths>
#include <QKeyEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QMetaEnum>
#include <QPainter>
#include <QWidget>

#include "Constants/constants.h"
#include "GUI/Animations/linear_emulator.h"
#include "Settings/settings.h"
#include "key_names.h"

namespace ControlsWrapper {

Q_NAMESPACE

enum class Controls {
  kUp,
  kDown,
  kLeft,
  kRight,
  kReload,
  kShowStatistics,
  kShowNetGraph,
  kRespawn,
  kShoot
};

Q_ENUM_NS(Controls)

}  // namespace ControlsWrapper

using Controls = ControlsWrapper::Controls;

Q_DECLARE_METATYPE(Controls)

namespace Constants::KeyController {

const std::vector<QString> kControlsNames{
  "Up",
  "Down",
  "Left",
  "Right",
  "Reload",
  "Show Statistics",
  "Show Net Graph",
  "Respawn",
  "Shoot"
};

const float kSettingHeight = 30.f;
const float kBlankSpaceBetweenSettings = 10.f;
const float kInnerOffsetX = 30.f;
const float kInnerOffsetY = 30.f;
const float kAnimationSpeed = 5.f;
const int kAnimationMaxMsecs = 500;
const QColor kHighlightedColor(115, 237, 64, 210);
const QColor kDefaultSettingColor(126, 210, 91, 210);
const QColor kEmptySettingColor(241, 105, 59, 210);
const QColor kBackgroundColor(86, 86, 86, 180);

}  // namespace Constants::KeyController

struct NativeButton {
  NativeButton() = delete;
  NativeButton(bool is_keyboard_, uint32_t control_);
  explicit NativeButton(const QVariant& variant);
  explicit NativeButton(QKeyEvent* key_event);
  explicit NativeButton(QMouseEvent* mouse_event);

  QVariant ToQVariant() const;
  QString GetButtonName() const;

  bool operator<(const NativeButton& other) const;
  bool is_keyboard;
  uint32_t control;
};

class KeyController : public QWidget {
  Q_OBJECT

 public:
  explicit KeyController(QWidget* parent);

  void AddKeyPressEvent(QKeyEvent* key_event);
  void AddKeyReleaseEvent(QKeyEvent* key_event);
  void AddMousePressEvent(QMouseEvent* mouse_event);
  void AddMouseReleaseEvent(QMouseEvent* mouse_event);

  void NativeButtonPressedEvent(const NativeButton& native_button);
  void NativeButtonReleasedEvent(const NativeButton& native_button);

  bool IsHeld(Controls control);
  bool WasPressed(Controls control);
  void ClearKeyPress(Controls control);

  void ClearControls();

  void paintEvent(QPaintEvent* paint_event) override;
  void mousePressEvent(QMouseEvent* mouse_event) override;
  void keyPressEvent(QKeyEvent* key_event) override;

  bool IsShown() const;
  void Hide();
  void Show();

  void ReadSettings();
  void SaveSettings() const;

 private:
  std::unordered_map<Controls, QString> control_to_key_name_{};
  std::map<NativeButton, Controls> native_button_to_control_{
#ifdef WIN32
      {{true, 17}, Controls::kUp},
      {{true, 31}, Controls::kDown},
      {{true, 30}, Controls::kLeft},
      {{true, 32}, Controls::kRight},
      {{true, 19}, Controls::kReload},
      {{true, 15}, Controls::kShowStatistics},
      {{true, 61}, Controls::kShowNetGraph},
      {{true, 46}, Controls::kRespawn},
      {{false, 1}, Controls::kShoot}
#else
      {{true, 25}, Controls::kUp},
      {{true, 39}, Controls::kDown},
      {{true, 38}, Controls::kLeft},
      {{true, 40}, Controls::kRight},
      {{true, 27}, Controls::kReload},
      {{true, 23}, Controls::kShowStatistics},
      {{true, 69}, Controls::kShowNetGraph},
      {{true, 54}, Controls::kRespawn},
      {{false, 1}, Controls::kShoot}
#endif
  };

  static int64_t GetCurrentTime();
  void UnbindControlFromNativeButton(Controls control);
  void BindNativeButtonToControl(Controls control,
                                 const NativeButton& native_button,
                                 const QString& button_name);

  std::unordered_map<Controls, int64_t> last_pressed_;
  std::unordered_map<Controls, int64_t> last_released_;
  std::unordered_map<Controls, bool> is_held_;
  std::unordered_map<Controls, bool> was_pressed_;

  std::vector<QRectF> settings_rects_;
  int highlighted_setting_index_{-1};

  bool is_shown_{false};

  LinearEmulator<float> opacity_emulator_;
  QTimer hide_timer_;
  QGraphicsOpacityEffect* opacity_effect;
};

#endif  // CLIENT_KEYCONTROLLER_KEY_CONTROLLER_H_
