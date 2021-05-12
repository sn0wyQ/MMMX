#include "reloading_field.h"

using Constants::ReloadingField::kBulletWidth;
using Constants::ReloadingField::kBulletHeight;
using Constants::ReloadingField::kMaxInColumns;
using Constants::ReloadingField::kMaxInRows;
using Constants::ReloadingField::kSpaceBetweenBullets;

ReloadingField::ReloadingField(QWidget* parent,
                               std::shared_ptr<ClientController> controller) :
    QWidget(parent), controller_{std::move(controller)} {
}

void ReloadingField::paintEvent(QPaintEvent* paint_event) {
  if (!controller_->GetModel()->IsLocalPlayerSet()) {
    return;
  }

  QPainter painter(this);

  QString base_path = "./Res/Pictures/ReloadingField/";
  QSvgRenderer renderer_empty_bullet(base_path + "empty_bullet.svg");
  QSvgRenderer renderer_bullet(base_path + "bullet.svg");
  bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
  empty_bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
  bullet_pixmap_.fill(Qt::transparent);
  empty_bullet_pixmap_.fill(Qt::transparent);
  QPainter painter_for_bullet_pixmap(&bullet_pixmap_);
  QPainter painter_for_empty_bullet_pixmap(&empty_bullet_pixmap_);
  renderer_bullet.render(&painter_for_bullet_pixmap, bullet_pixmap_.rect());
  renderer_empty_bullet.render(&painter_for_empty_bullet_pixmap,
                               empty_bullet_pixmap_.rect());

  this->RecalculateFields();

  auto local_player_weapon =
      controller_->GetModel()->GetLocalPlayer()->GetWeapon();

  int64_t last_time_pressed_reload =
      local_player_weapon->GetLastTimePressedReload();
  int64_t reloading_time = local_player_weapon->GetReloadingTime();
  int64_t current_time = controller_->GetCurrentServerTime();

  if (current_time - last_time_pressed_reload <= reloading_time) {
    this->DrawReload(&painter,
                     current_time - last_time_pressed_reload,
                     reloading_time);
  } else {
    this->Draw(&painter);
  }
}

void ReloadingField::RecalculateFields() {
  in_rows_ = this->width() / (kSpaceBetweenBullets + kBulletWidth);
  int clip_size =
      controller_->GetModel()->GetLocalPlayer()->GetWeapon()->GetClipSize();
  in_columns_ = (clip_size + in_rows_ - 1) / in_rows_;
}

void ReloadingField::Draw(QPainter* painter) {
  painter->save();

  int bullets_in_clip = controller_->GetModel()->GetLocalPlayer()->GetWeapon()
      ->GetCurrentBulletsInClip();
  this->DrawPixmaps(painter, bullets_in_clip);

  painter->restore();
}

void ReloadingField::DrawReload(QPainter* painter,
                                int64_t delta_time,
                                int64_t reloading_time) {
  painter->save();

  int bullets_in_clip = in_rows_ * in_columns_ * delta_time / reloading_time;
  this->DrawPixmaps(painter, bullets_in_clip);

  painter->restore();
}

void ReloadingField::DrawPixmaps(QPainter* painter, int bullets_in_clip) {
  painter->save();

  int real_height = static_cast<int>(static_cast<float>(in_columns_)
      * (kSpaceBetweenBullets + kBulletHeight));
  int real_width = static_cast<int>(static_cast<float>(in_rows_)
      * (kSpaceBetweenBullets + kBulletWidth));
  int shift_x = this->size().width() - real_width;
  int shift_y = this->size().height() - real_height;
  int clip_size =
      controller_->GetModel()->GetLocalPlayer()->GetWeapon()->GetClipSize();
  int max_clip_size = in_columns_ * in_rows_;
  int delta = max_clip_size - clip_size;
  int current_bullet = 0;

  while (current_bullet < max_clip_size) {
    int x = static_cast<int>(static_cast<float>((current_bullet % in_rows_))
        * (kBulletWidth + kSpaceBetweenBullets));
    int y = static_cast<int>(static_cast<float>((current_bullet / in_rows_))
        * (kBulletHeight + kSpaceBetweenBullets));
    if (current_bullet < delta) {
      current_bullet++;
      continue;
    }
    if (current_bullet < max_clip_size - bullets_in_clip ) {
      painter->drawPixmap(shift_x + x, shift_y + y, empty_bullet_pixmap_);
    } else {
      painter->drawPixmap(shift_x + x, shift_y + y, bullet_pixmap_);
    }
    current_bullet++;
  }

  painter->restore();
}
