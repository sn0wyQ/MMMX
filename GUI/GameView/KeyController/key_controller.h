#ifndef GUI_GAMEVIEW_KEYCONTROLLER_KEY_CONTROLLER_H_
#define GUI_GAMEVIEW_KEYCONTROLLER_KEY_CONTROLLER_H_

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDateTime>

#include "constants.h"
#include "GUI/Animations/linear_emulator.h"

enum class Key {
  kUp,
  kDown,
  kLeft,
  kRight,
  kReload,
  kShowStatistics,
  kRespawn,
  kShoot
};

namespace Constants::KeySettings {

const std::vector<QString> kSettingNames{
  "Up",
  "Down",
  "Left",
  "Right",
  "Reload",
  "Show Statistics",
  "Respawn",
  "Shoot"
};

const float kSettingHeight = 30.f;
const float kBlankSpaceBetweenSettings = 10.f;
const float kInnerOffsetX = 30.f;
const float kInnerOffsetY = 30.f;

}  // namespace Constants::KeySettings

struct NativeButton {
  NativeButton() = delete;
  NativeButton(bool is_keyboard_, uint32_t key_);
  explicit NativeButton(QKeyEvent* key_event);
  explicit NativeButton(QMouseEvent* mouse_event);

  bool operator<(const NativeButton& other) const;
  bool is_keyboard;
  uint32_t key;
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

  bool IsHeld(Key key);
  bool WasPressed(Key key);
  void ClearKeyPress(Key key);

  void ClearControls();

  void paintEvent(QPaintEvent* paint_event) override;
  void mousePressEvent(QMouseEvent* mouse_event) override;
  void keyPressEvent(QKeyEvent* key_event) override;

  void Hide();
  void Show();

 private:
  std::unordered_map<Key, QString> key_to_key_name_{
      {Key::kUp, "W"},
      {Key::kDown, "S"},
      {Key::kLeft, "A"},
      {Key::kRight, "D"},
      {Key::kReload, "R"},
      {Key::kShowStatistics, "TAB"},
      {Key::kRespawn, "C"},
      {Key::kShoot, "LMouse"}
  };
  std::map<NativeButton, Key> native_button_to_key_{
#ifdef WIN32
      {{true, 17}, Key::kUp},
      {{true, 31}, Key::kDown},
      {{true, 30}, Key::kLeft},
      {{true, 32}, Key::kRight},
      {{true, 19}, Key::kReload},
      {{true, 15}, Key::kShowStatistics},
      {{true, 46}, Key::kRespawn},
      {{false, 1}, Key::kShoot}
#else
      {{true, 25}, Key::kUp},
      {{true, 39}, Key::kDown},
      {{true, 38}, Key::kLeft},
      {{true, 40}, Key::kRight},
      {{true, 27}, Key::kReload},
      {{true, 23}, Key::kShowStatistics},
      {{true, 54}, Key::kRespawn},
      {{false, 1}, Key::kShoot}
#endif
  };

  static int64_t GetCurrentTime();
  void UnbindKeyFromNativeButton(Key key);
  void BindNativeButtonToKey(Key key,
                             const NativeButton& native_button,
                             const QString& button_name);

  std::unordered_map<Key, int64_t> last_pressed_;
  std::unordered_map<Key, int64_t> last_released_;
  std::unordered_map<Key, bool> is_held_;
  std::unordered_map<Key, bool> was_pressed_;

  std::vector<QRectF> settings_rects_;
  int highlighted_setting_index_{-1};

  float opacity_target_;
  LinearEmulator<float> opacity_emulator_;
};

#endif  // GUI_GAMEVIEW_KEYCONTROLLER_KEY_CONTROLLER_H_
