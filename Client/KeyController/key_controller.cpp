#include "key_controller.h"

using Constants::KeySettings::kBlankSpaceBetweenSettings;
using Constants::KeySettings::kSettingHeight;
using Constants::KeySettings::kSettingNames;
using Constants::KeySettings::kInnerOffsetX;
using Constants::KeySettings::kInnerOffsetY;
using Constants::KeySettings::kHighlightedColor;
using Constants::KeySettings::kDefaultSettingColor;
using Constants::KeySettings::kEmptySettingColor;
using Constants::KeySettings::kBackgroundColor;
using Constants::KeySettings::kAnimationSpeed;
using Constants::KeySettings::kAnimationMaxMsecs;

KeyController::KeyController(QWidget* parent) :
    QWidget(parent),
    opacity_emulator_(kAnimationSpeed),
    hide_timer_(this),
    opacity_effect(new QGraphicsOpacityEffect(this)) {
  this->setMouseTracking(true);
  this->ReadSettings();
  for (auto[native_button, key] : native_button_to_key_) {
    key_to_key_name_[key] = native_button.GetButtonName();
  }
  opacity_emulator_.SetStopOnMax(true);
  opacity_emulator_.SetStopOnMin(true);
  opacity_emulator_.SetCurrentValue(0.01f);
  opacity_emulator_.SetPath(255.f, 0.01f);
  this->setGraphicsEffect(opacity_effect);
  hide_timer_.setInterval(kAnimationMaxMsecs);
  hide_timer_.setSingleShot(true);
  connect(&hide_timer_, &QTimer::timeout,
          this, &KeyController::hide);
}

void KeyController::paintEvent(QPaintEvent* paint_event) {
  opacity_effect->setOpacity(opacity_emulator_.GetCurrentValue() / 255.f);

  int settings_count = kSettingNames.size();
  QPixmap canvas(this->width(),
                 (kSettingHeight + kBlankSpaceBetweenSettings)
                     * settings_count + 2 * kInnerOffsetY
                     - kBlankSpaceBetweenSettings);
  canvas.fill(Qt::transparent);
  QPainter canvas_painter(&canvas);
  Constants::SetPainterHints(&canvas_painter);
  settings_rects_.clear();
  canvas_painter.translate(kInnerOffsetX, kInnerOffsetY);
  for (int i = 0; i < settings_count; i++) {
    auto setting_rect = QRectF(0, 0, this->width() - 2 * kInnerOffsetX,
                               kSettingHeight);
    settings_rects_.emplace_back(QPointF(kInnerOffsetX,
                                         i * (kSettingHeight
                                             + kBlankSpaceBetweenSettings)
                                             + kInnerOffsetY),
                                 QPointF(this->width() - kInnerOffsetX,
                                         i * (kSettingHeight
                                             + kBlankSpaceBetweenSettings)
                                             + kSettingHeight + kInnerOffsetY));
    auto key = static_cast<Key>(i);
    if (highlighted_setting_index_ == i) {
      canvas_painter.setBrush(QBrush(kHighlightedColor));
    } else if (key_to_key_name_[key].isEmpty()) {
      canvas_painter.setBrush(QBrush(kEmptySettingColor));
    } else {
      canvas_painter.setBrush(QBrush(kDefaultSettingColor));
    }
    canvas_painter.setPen(QPen(Qt::black, 2));
    canvas_painter.drawRoundedRect(setting_rect, 10, 10);
    canvas_painter.drawText(QRectF(0,
                                   0,
                                   this->width() / 2 - kInnerOffsetX,
                                   kSettingHeight),
                            Qt::AlignCenter, kSettingNames[i]);
    canvas_painter.drawText(QRectF(this->width() / 2, 0,
                                   this->width() / 2, kSettingHeight),
                            Qt::AlignCenter,
                            key_to_key_name_[key]);
    canvas_painter.translate(
        QPointF(0, kSettingHeight + kBlankSpaceBetweenSettings));
  }
  QPainter painter(this);
  Constants::SetPainterHints(&painter);
  painter.setBrush(kBackgroundColor);
  painter.setPen(QPen(Qt::transparent));
  painter.drawRoundedRect(0,
                          0,
                          this->width(),
                          canvas.height(),
                          kInnerOffsetX,
                          kInnerOffsetY);
  painter.drawPixmap(QRectF(0, 0, this->width(), canvas.height()),
                     canvas,
                     QRectF(0, 0, this->width(), canvas.height()));
}

void KeyController::AddKeyPressEvent(QKeyEvent* key_event) {
  this->NativeButtonPressedEvent(NativeButton(key_event));
}

void KeyController::AddKeyReleaseEvent(QKeyEvent* key_event) {
  this->NativeButtonReleasedEvent(NativeButton(key_event));
}

void KeyController::AddMousePressEvent(QMouseEvent* mouse_event) {
  this->NativeButtonPressedEvent(NativeButton(mouse_event));
}

void KeyController::AddMouseReleaseEvent(QMouseEvent* mouse_event) {
  this->NativeButtonReleasedEvent(NativeButton(mouse_event));
}

void KeyController::NativeButtonPressedEvent(
    const NativeButton& native_button) {
  if (native_button_to_key_.find(native_button) ==
      native_button_to_key_.end()) {
    return;
  }
  auto key = native_button_to_key_[native_button];
  last_pressed_[key] = this->GetCurrentTime();
  is_held_[key] = true;
  was_pressed_[key] = true;
}

void KeyController::NativeButtonReleasedEvent(
    const NativeButton& native_button) {
  if (native_button_to_key_.find(native_button) ==
      native_button_to_key_.end()) {
    return;
  }
  auto key = native_button_to_key_[native_button];
  last_released_[key] = this->GetCurrentTime();
}

bool KeyController::IsHeld(Key key) {
  if (this->IsShown()) {
    return false;
  }
  if (is_held_[key] &&
      last_pressed_[key] < last_released_[key] &&
      GetCurrentTime() - last_released_[key] > 50) {
    is_held_[key] = false;
  }
  return is_held_[key];
}

bool KeyController::WasPressed(Key key) {
  if (this->IsShown()) {
    return false;
  }
  return was_pressed_[key];
}

void KeyController::ClearKeyPress(Key key) {
  was_pressed_[key] = false;
}

int64_t KeyController::GetCurrentTime() {
  return QDateTime::currentMSecsSinceEpoch();
}

void KeyController::mousePressEvent(QMouseEvent* mouse_event) {
  if (!this->IsShown()) {
    mouse_event->setAccepted(false);
    return;
  }
  auto pos = mouse_event->pos();
  int setting_index = -1;
  for (size_t i = 0; i < settings_rects_.size(); i++) {
    if (settings_rects_[i].contains(pos)) {
      setting_index = i;
      break;
    }
  }
  if (mouse_event->button() == Qt::MouseButton::LeftButton) {
    if (highlighted_setting_index_ == -1) {
      highlighted_setting_index_ = setting_index;
      return;
    }
    if (highlighted_setting_index_ == setting_index) {
      highlighted_setting_index_ = -1;
      return;
    }
  }
  auto key = static_cast<Key>(highlighted_setting_index_);
  this->UnbindKeyFromNativeButton(key);
  NativeButton native_button(mouse_event);
  this->BindNativeButtonToKey(key, native_button,
                              native_button.GetButtonName());
  highlighted_setting_index_ = -1;
}

void KeyController::keyPressEvent(QKeyEvent* key_event) {
  if (highlighted_setting_index_ == -1 ||
      key_event->key() == Qt::Key_Escape ||
      key_event->key() == Qt::Key_F1) {
    highlighted_setting_index_ = -1;
    return;
  }
  auto key = static_cast<Key>(highlighted_setting_index_);
  this->UnbindKeyFromNativeButton(key);
  NativeButton native_button(key_event);
  this->BindNativeButtonToKey(key, native_button,
                              native_button.GetButtonName());
  highlighted_setting_index_ = -1;
}

void KeyController::ClearControls() {
  for (auto&[key, is_held] : is_held_) {
    is_held = false;
  }
  for (auto&[key, was_pressed] : was_pressed_) {
    was_pressed = false;
  }
}

void KeyController::UnbindKeyFromNativeButton(Key key) {
  for (auto[code, key_] : native_button_to_key_) {
    if (key_ == key) {
      native_button_to_key_.erase(code);
      break;
    }
  }
}

void KeyController::BindNativeButtonToKey(Key key,
                                          const NativeButton& native_button,
                                          const QString& button_name) {
  if (native_button_to_key_.find(native_button) !=
      native_button_to_key_.end()) {
    key_to_key_name_.erase(native_button_to_key_[native_button]);
  }
  this->UnbindKeyFromNativeButton(key);
  key_to_key_name_[key] = button_name;
  native_button_to_key_[native_button] = key;
}

void KeyController::Hide() {
  is_shown_ = false;
  opacity_emulator_.SetPath(255.f, 0.01f);
  hide_timer_.stop();
  hide_timer_.start();
}

void KeyController::Show() {
  this->ClearControls();
  is_shown_ = true;
  this->show();
  opacity_emulator_.SetPath(0.01f, 255.f);
  hide_timer_.stop();
}

bool KeyController::IsShown() const {
  return is_shown_;
}

void KeyController::ReadSettings() {
  auto location = QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppConfigLocation);
  QSettings settings(location, QSettings::Format::IniFormat);
  for (int i = 0; i < kSettingNames.size(); i++) {
    auto key = static_cast<Key>(i);
    auto settings_key =
        "key_settings/" + Constants::GetStringFromEnumValue(key);
    if (settings.contains(settings_key)) {
      this->UnbindKeyFromNativeButton(key);
      native_button_to_key_[NativeButton(settings.value(settings_key))] = key;
    }
  }
}

void KeyController::SaveSettings() const {
  auto location = QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppConfigLocation);
  QSettings settings(location, QSettings::Format::IniFormat);
  for (auto&[native_button, key] : native_button_to_key_) {
    settings.setValue("key_settings/" + Constants::GetStringFromEnumValue(key),
                      native_button.ToQVariant());
  }
}

NativeButton::NativeButton(bool is_keyboard_, uint32_t key_) :
    is_keyboard(is_keyboard_),
    key(key_) {}

NativeButton::NativeButton(QKeyEvent* key_event) :
    is_keyboard(true),
    key(key_event->nativeScanCode()) {}

NativeButton::NativeButton(QMouseEvent* mouse_event) :
    is_keyboard(false),
    key(mouse_event->button()) {}

bool NativeButton::operator<(const NativeButton& other) const {
  if (is_keyboard != other.is_keyboard) {
    return !is_keyboard;
  }
  return key < other.key;
}

QString NativeButton::GetButtonName() const {
  if (is_keyboard) {
    if (KeyNames::kNativeCodeToKeyName.find(key)
        != KeyNames::kNativeCodeToKeyName.end()) {
      return KeyNames::kNativeCodeToKeyName.at(key);
    }
    return "Unknown";
  }
  return "Mouse " + QString::number(key);
}

NativeButton::NativeButton(const QVariant& variant) {
  is_keyboard = variant.toStringList()[0].toInt();
  key = variant.toStringList()[1].toInt();
}

QVariant NativeButton::ToQVariant() const {
  return QVariant(QStringList{QString::number(is_keyboard),
                              QString::number(key)});
}
