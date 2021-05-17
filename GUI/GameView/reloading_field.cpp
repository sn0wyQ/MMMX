#include "reloading_field.h"

using Constants::ReloadingField::kBulletWidth;
using Constants::ReloadingField::kBulletHeight;
using Constants::ReloadingField::kMinSpaceBetweenBullets;

ReloadingField::ReloadingField(QWidget* parent,
                               std::shared_ptr<ClientController> controller) :
    QWidget(parent), controller_{std::move(controller)} {
    QString base_path = "./Res/Icons/";
    QSvgRenderer renderer_empty_bullet(base_path + "EmptyBullet.svg");
    QSvgRenderer renderer_bullet(base_path + "GoldBullet.svg");
    bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
    empty_bullet_pixmap_ = QPixmap(kBulletWidth, kBulletHeight);
    bullet_pixmap_.fill(Qt::transparent);
    empty_bullet_pixmap_.fill(Qt::transparent);
    QPainter painter_for_bullet_pixmap(&bullet_pixmap_);
    QPainter painter_for_empty_bullet_pixmap(&empty_bullet_pixmap_);
    renderer_bullet.render(&painter_for_bullet_pixmap, bullet_pixmap_.rect());
    renderer_empty_bullet.render(&painter_for_empty_bullet_pixmap,
                                 empty_bullet_pixmap_.rect());
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
    in_rows_ = clip_size;
    in_columns_ = 1;
  } else if (max_in_row * 2 >= clip_size) {
    in_rows_ = clip_size / 2;
    in_columns_ = 2;
  } else {
    in_columns_ =  (clip_size + max_in_row - 1) / max_in_row;
    in_rows_ = (clip_size + in_columns_ - 1) / in_columns_;
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

  int real_height = static_cast<int>(static_cast<float>(in_columns_)
      * (kMinSpaceBetweenBullets + kBulletHeight));
  int real_width = static_cast<int>(static_cast<float>(in_rows_)
      * (kMinSpaceBetweenBullets + kBulletWidth));
  int shift_x = this->size().width() - real_width;
  int shift_y = this->size().height() - real_height;
  int max_clip_size = in_columns_ * in_rows_;
  int delta = max_clip_size - clip_size_;
  int current_bullet = 0;

  while (current_bullet < max_clip_size) {
    int x = static_cast<int>(static_cast<float>((current_bullet % in_rows_))
        * (kBulletWidth + kMinSpaceBetweenBullets));
    int y = static_cast<int>(static_cast<float>((current_bullet / in_rows_))
        * (kBulletHeight + kMinSpaceBetweenBullets));
    if (current_bullet < delta) {
      current_bullet++;
      continue;
    }
    if (current_bullet < max_clip_size - bullets_in_clip) {
      painter->drawPixmap(shift_x + x, shift_y + y, empty_bullet_pixmap_);
    } else {
      painter->drawPixmap(shift_x + x, shift_y + y, bullet_pixmap_);
    }
    current_bullet++;
  }

  painter->restore();
}
