#include "reloading_field.h"

using Constants::ReloadingField::kBulletWidth;
using Constants::ReloadingField::kBulletHeight;
using Constants::ReloadingField::kMinSpaceBetweenBullets;

ReloadingField::ReloadingField(QWidget* parent, ClientController* controller)
    : QWidget(parent), controller_(controller) {
  QString base_path = "./Res/Icons/";
  QSvgRenderer unfilled_bullet_renderer(base_path + "unfilled_bullet.svg");
  QSvgRenderer filled_bullet_renderer(base_path + "filled_bullet.svg");
  filled_bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
  unfilled_bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
  filled_bullet_pixmap_.fill(Qt::transparent);
  unfilled_bullet_pixmap_.fill(Qt::transparent);
  QPainter painter_for_filled_bullet_pixmap(&filled_bullet_pixmap_);
  QPainter painter_for_unfilled_bullet_pixmap(&unfilled_bullet_pixmap_);
  filled_bullet_renderer.render(&painter_for_filled_bullet_pixmap,
                                filled_bullet_pixmap_.rect());
  unfilled_bullet_renderer.render(&painter_for_unfilled_bullet_pixmap,
                                  unfilled_bullet_pixmap_.rect());
}

void ReloadingField::paintEvent(QPaintEvent* paint_event) {
  if (!controller_->GetModel()->IsLocalPlayerSet()) {
    return;
  }

  QPainter painter(this);

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
  int clip_size =
      controller_->GetModel()->GetLocalPlayer()->GetWeapon()->GetClipSize();
  int max_in_row = this->width() / (kMinSpaceBetweenBullets + kBulletWidth);
  clip_size_ = clip_size;
  if (max_in_row >= clip_size) {
    bullet_columns_ = clip_size;
    bullet_rows_ = 1;
  } else if (max_in_row * 2 >= clip_size) {
    bullet_columns_ = clip_size / 2;
    bullet_rows_ = 2;
  } else {
    bullet_rows_ =  (clip_size + max_in_row - 1) / max_in_row;
    bullet_columns_ = (clip_size + bullet_rows_ - 1) / bullet_rows_;
  }
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

  int bullets_in_clip = clip_size_ * delta_time / reloading_time;
  this->DrawPixmaps(painter, bullets_in_clip);

  painter->restore();
}

void ReloadingField::DrawPixmaps(QPainter* painter, int bullets_in_clip) {
  painter->save();

  int real_height = static_cast<int>(static_cast<float>(bullet_rows_)
      * (kMinSpaceBetweenBullets + kBulletHeight));
  int real_width = static_cast<int>(static_cast<float>(bullet_columns_)
      * (kMinSpaceBetweenBullets + kBulletWidth));
  int shift_x = this->size().width() - real_width;
  int shift_y = this->size().height() - real_height;
  int max_clip_size = bullet_rows_ * bullet_columns_;
  int delta = max_clip_size - clip_size_;
  int current_bullet = 0;

  while (current_bullet < max_clip_size) {
    int x =
        static_cast<int>(static_cast<float>((current_bullet % bullet_columns_))
            * (kBulletWidth + kMinSpaceBetweenBullets));
    int y =
        static_cast<int>(static_cast<float>((current_bullet / bullet_columns_))
            * (kBulletHeight + kMinSpaceBetweenBullets));
    if (current_bullet < delta) {
      current_bullet++;
      continue;
    }
    if (current_bullet < max_clip_size - bullets_in_clip) {
      painter->drawPixmap(shift_x + x, shift_y + y, unfilled_bullet_pixmap_);
    } else {
      painter->drawPixmap(shift_x + x, shift_y + y, filled_bullet_pixmap_);
    }
    current_bullet++;
  }

  painter->restore();
}
