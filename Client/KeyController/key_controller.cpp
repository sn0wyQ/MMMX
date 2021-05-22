#include "key_controller.h"

using Constants::KeyController::kBlankSpaceBetweenSettings;
using Constants::KeyController::kSettingHeight;
using Constants::KeyController::kControlsNames;
using Constants::KeyController::kInnerOffsetX;
using Constants::KeyController::kInnerOffsetY;
using Constants::KeyController::kHighlightedColor;
using Constants::KeyController::kDefaultSettingColor;
using Constants::KeyController::kEmptySettingColor;
using Constants::KeyController::kBackgroundColor;
using Constants::KeyController::kAnimationSpeed;
using Constants::KeyController::kAnimationMaxMsecs;

KeyController::KeyController(QWidget* parent) :
    QWidget(parent),
    opacity_emulator_(kAnimationSpeed),
    hide_timer_(this),
    opacity_effect(new QGraphicsOpacityEffect(this)) {
  this->setMouseTracking(true);
  this->ReadSettings();
  for (auto[native_button, control] : native_button_to_control_) {
    control_to_key_name_[control] = native_button.GetButtonName();
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

  int settings_count = kControlsNames.size();
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
    auto control = static_cast<Controls>(i);
    if (highlighted_setting_index_ == i) {
      canvas_painter.setBrush(QBrush(kHighlightedColor));
    } else if (control_to_key_name_[control].isEmpty()) {
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
                            Qt::AlignCenter, kControlsNames[i]);
    canvas_painter.drawText(QRectF(this->width() / 2, 0,
                                   this->width() / 2, kSettingHeight),
                            Qt::AlignCenter,
                            control_to_key_name_[control]);
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
  if (native_button_to_control_.find(native_button) ==
      native_button_to_control_.end()) {
    return;
  }
  auto control = native_button_to_control_[native_button];
  last_pressed_[control] = this->GetCurrentTime();
  is_held_[control] = true;
  was_pressed_[control] = true;
}

void KeyController::NativeButtonReleasedEvent(
    const NativeButton& native_button) {
  if (native_button_to_control_.find(native_button) ==
      native_button_to_control_.end()) {
    return;
  }
  auto control = native_button_to_control_[native_button];
  last_released_[control] = GetCurrentTime();
  is_held_[control] = false;
}

bool KeyController::IsHeld(Controls control) {
  if (this->IsShown()) {
    return false;
  }
  return is_held_[control];
}

bool KeyController::WasPressed(Controls control) {
  if (this->IsShown()) {
    return false;
  }
  return was_pressed_[control];
}

void KeyController::ClearKeyPress(Controls control) {
  was_pressed_[control] = false;
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
  auto control = static_cast<Controls>(highlighted_setting_index_);
  this->UnbindControlFromNativeButton(control);
  NativeButton native_button(mouse_event);
  this->BindNativeButtonToControl(control, native_button,
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
  auto control = static_cast<Controls>(highlighted_setting_index_);
  this->UnbindControlFromNativeButton(control);
  NativeButton native_button(key_event);
  this->BindNativeButtonToControl(control, native_button,
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

void KeyController::UnbindControlFromNativeButton(Controls control) {
  for (auto[code, control_] : native_button_to_control_) {
    if (control_ == control) {
      native_button_to_control_.erase(code);
      break;
    }
  }
}

void KeyController::BindNativeButtonToControl(Controls control,
                                              const NativeButton& native_button,
                                              const QString& button_name) {
  if (native_button_to_control_.find(native_button) !=
      native_button_to_control_.end()) {
    control_to_key_name_.erase(native_button_to_control_[native_button]);
  }
  this->UnbindControlFromNativeButton(control);
  control_to_key_name_[control] = button_name;
  native_button_to_control_[native_button] = control;
  this->SaveSettings();
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
  for (size_t i = 0; i < kControlsNames.size(); i++) {
    auto control = static_cast<Controls>(i);
    auto settings_key =
        "key_settings/" + Constants::GetStringFromEnumValue(control);
    if (settings.contains(settings_key)) {
      this->UnbindControlFromNativeButton(control);
      native_button_to_control_[
          NativeButton(settings.value(settings_key))] = control;
    }
  }
}

void KeyController::SaveSettings() const {
  auto location = QStandardPaths::writableLocation(
      QStandardPaths::StandardLocation::AppConfigLocation);
  QSettings settings(location, QSettings::Format::IniFormat);
  for (auto&[native_button, control] : native_button_to_control_) {
    settings.setValue("key_settings/" +
                          Constants::GetStringFromEnumValue(control),
                      native_button.ToQVariant());
  }
}

NativeButton::NativeButton(bool is_keyboard_, uint32_t control_) :
    is_keyboard(is_keyboard_),
    control(control_) {}

NativeButton::NativeButton(QKeyEvent* key_event) :
    is_keyboard(true),
    control(key_event->nativeScanCode()) {}

NativeButton::NativeButton(QMouseEvent* mouse_event) :
    is_keyboard(false),
    control(mouse_event->button()) {}

bool NativeButton::operator<(const NativeButton& other) const {
  if (is_keyboard != other.is_keyboard) {
    return !is_keyboard;
  }
  return control < other.control;
}

QString NativeButton::GetButtonName() const {
  if (is_keyboard) {
    if (KeyNames::kNativeCodeToKeyName.find(control)
        != KeyNames::kNativeCodeToKeyName.end()) {
      return KeyNames::kNativeCodeToKeyName.at(control);
    }
    return "Unknown";
  }
  return "Mouse " + QString::number(control);
}

NativeButton::NativeButton(const QVariant& variant) {
  is_keyboard = variant.toStringList()[0].toInt();
  control = variant.toStringList()[1].toInt();
}

QVariant NativeButton::ToQVariant() const {
  return QVariant(QStringList{QString::number(is_keyboard),
                              QString::number(control)});
}
