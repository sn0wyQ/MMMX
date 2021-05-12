#include <QDebug>
#include <QPainter>
#include "key_controller.h"

using Constants::KeySettings::kBlankSpaceBetweenSettings;
using Constants::KeySettings::kSettingHeight;
using Constants::KeySettings::kSettingNames;
using Constants::KeySettings::kInnerOffsetX;
using Constants::KeySettings::kInnerOffsetY;

KeyController::KeyController(QWidget* parent) :
    QWidget(parent),
    opacity_emulator_(0.01f) {
  this->setMouseTracking(true);

}

void KeyController::paintEvent(QPaintEvent* paint_event) {
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
    if (highlighted_setting_index_ == i) {
      canvas_painter.setBrush(QBrush(Qt::green));
    } else {
      canvas_painter.setBrush(QBrush(Qt::cyan));
    }
    canvas_painter.drawRoundedRect(setting_rect, 10, 10);
    canvas_painter.drawText(QRectF(0,
                                   0,
                                   this->width() / 2 - kInnerOffsetX,
                                   kSettingHeight),
                            Qt::AlignCenter, kSettingNames[i]);
    canvas_painter.drawText(QRectF(this->width() / 2, 0,
                                   this->width() / 2, kSettingHeight),
                            Qt::AlignCenter,
                            key_to_key_name_[static_cast<Key>(i)]);
    canvas_painter.translate(
        QPointF(0, kSettingHeight + kBlankSpaceBetweenSettings));
  }
  QPainter painter(this);
  Constants::SetPainterHints(&painter);
  painter.setBrush(Qt::darkBlue);
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
  last_pressed_[key] = GetCurrentTime();
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
  last_released_[key] = GetCurrentTime();
}

bool KeyController::IsHeld(Key key) {
  if (!this->isHidden()) {
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
  if (!this->isHidden()) {
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
  auto pos = mouse_event->pos();
  int setting_index = -1;
  for (size_t i = 0; i < settings_rects_.size(); i++) {
    if (settings_rects_[i].contains(pos)) {
      setting_index = i;
      break;
    }
  }
  if (highlighted_setting_index_ == -1 &&
      mouse_event->button() == Qt::MouseButton::LeftButton) {
    highlighted_setting_index_ = setting_index;
    return;
  }
  if (highlighted_setting_index_ == -1) {
    return;
  }
  if (highlighted_setting_index_ == setting_index &&
      mouse_event->button() == Qt::MouseButton::LeftButton) {
    highlighted_setting_index_ = -1;
    return;
  }

  this->BindNativeButtonToKey(static_cast<Key>(highlighted_setting_index_),
                              NativeButton(mouse_event),
                              "Mouse"
                                  + QString::number(mouse_event->button()));
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
  auto text = key_event->text();
  if (text.isEmpty() || !text[0].isLetterOrNumber()) {
    text = "#" + QString::number(key_event->nativeScanCode());
  }
  this->BindNativeButtonToKey(key, NativeButton(key_event), text);
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

}

void KeyController::Show() {
  this->show();
  opacity_target_ = 255.f;
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
