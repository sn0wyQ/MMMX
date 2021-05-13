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

#include "constants.h"
#include "GUI/Animations/linear_emulator.h"
#include "key_names.h"

namespace KeyWrapper {

Q_NAMESPACE

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

Q_ENUM_NS(Key)

}  // namespace KeyWrapper

using Key = KeyWrapper::Key;

Q_DECLARE_METATYPE(Key)

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
const QColor kHighlightedColor(115, 237, 64, 210);
const QColor kDefaultSettingColor(126, 210, 91, 210);
const QColor kEmptySettingColor(241, 105, 59, 210);
const QColor kBackgroundColor(86, 86, 86, 180);

}  // namespace Constants::KeySettings

struct NativeButton {
  NativeButton() = delete;
  NativeButton(bool is_keyboard_, uint32_t key_);
  explicit NativeButton(const QVariant& variant);
  explicit NativeButton(QKeyEvent* key_event);
  explicit NativeButton(QMouseEvent* mouse_event);

  QVariant ToQVariant() const;
  QString GetButtonName() const;

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

  bool IsShown() const;
  void Hide();
  void Show();

  void ReadSettings();
  void SaveSettings() const;

 private:
  std::unordered_map<Key, QString> key_to_key_name_{};
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

  bool is_shown_{false};

  LinearEmulator<float> opacity_emulator_;
  QTimer hide_timer_;
  QGraphicsOpacityEffect* opacity_effect;
};

#endif  // CLIENT_KEYCONTROLLER_KEY_CONTROLLER_H_
