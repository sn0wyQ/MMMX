#include "reloading_field.h"

using Constants::ReloadingField::kDefaultBulletWidth;
using Constants::ReloadingField::kDefaultBulletHeight;
using Constants::ReloadingField::kDefaultMaxInColumns;
using Constants::ReloadingField::kDefaultMaxInRows;
using Constants::ReloadingField::kDefaultSpaceBetweenBullets;

ReloadingField::ReloadingField(QWidget* parent,
                               std::shared_ptr<ClientController> controller,
                               QPoint position,
                               QSize size) :
    QWidget(parent), controller_{std::move(controller)} {
  this->move(position);
  this->resize(size);
}

void ReloadingField::paintEvent(QPaintEvent* paint_event) {
  if (!controller_->GetModel()->IsLocalPlayerSet()) {
    return;
  }
  QPainter painter(this);
  if (!is_initialize_) {
    this->Initialize();
    this->RecalculateSize();
    is_initialize_ = true;
  }

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

void ReloadingField::Initialize() {
  QString base_path = "./Res/Pictures/ReloadingField/";
  QSvgRenderer renderer_empty_bullet(base_path + "empty_bullet.svg");
  QSvgRenderer renderer_bullet(base_path + "bullet.svg");
  bullet_ = QPixmap(kDefaultBulletWidth, kDefaultBulletHeight);
  empty_bullet_ = QPixmap(kDefaultBulletWidth, kDefaultBulletHeight);
  bullet_.fill(Qt::transparent);
  empty_bullet_.fill(Qt::transparent);
  QPainter painter1(&bullet_);
  QPainter painter2(&empty_bullet_);
  renderer_bullet.render(&painter1, bullet_.rect());
  renderer_empty_bullet.render(&painter2, empty_bullet_.rect());

  auto local_weapon = controller_->GetModel()->GetLocalPlayer()->GetWeapon();
  auto weapon_type = local_weapon->GetWeaponType();
  int clip_size = local_weapon->GetClipSize();
  switch (weapon_type) {
    case WeaponTypeWrapper::WeaponType::kAssaultRifle: {
      rows_ = 6;
      columns_ = clip_size / rows_;
      break;
    }

    case WeaponTypeWrapper::WeaponType::kCrossbow: {
      columns_ = 4;
      rows_ = clip_size / columns_;
      break;
    }

    case WeaponTypeWrapper::WeaponType::kMachineGun: {
      rows_ = 5;
      columns_ = clip_size / rows_;
      break;
    }

    case WeaponTypeWrapper::WeaponType::kShotgun: {
      columns_ = 3;
      rows_ = clip_size / columns_;
      break;
    }

    default: {
      qWarning() << "Addressing a nonexistent type of weapon\n";
      break;
    }
  }
}

void ReloadingField::RecalculateSize() {
  int new_height = static_cast<int>(static_cast<float>(rows_)
          * (kDefaultSpaceBetweenBullets + kDefaultBulletHeight));
  int new_width = static_cast<int>(static_cast<float>(columns_)
          * (kDefaultSpaceBetweenBullets + kDefaultBulletWidth));
  QSize old_size = this->size();
  this->resize(new_width, new_height);
  this->move(this->pos().x() + old_size.width() - this->size().width(),
             this->pos().y() + old_size.height() - this->size().height());
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

  int bullets_in_clip = columns_ * rows_ * delta_time / reloading_time;
  this->DrawPixmaps(painter, bullets_in_clip);

  painter->restore();
}

void ReloadingField::DrawPixmaps(QPainter* painter, int bullets_in_clip) {
  painter->save();

  int clip_size = columns_ * rows_;
  int current_bullet = 0;
  while (current_bullet < clip_size) {
    int x = static_cast<int>(static_cast<float>((current_bullet % columns_))
        * (kDefaultBulletWidth + kDefaultSpaceBetweenBullets));
    int y = static_cast<int>(static_cast<float>((current_bullet / columns_))
        * (kDefaultBulletHeight + kDefaultSpaceBetweenBullets));
    if (current_bullet < clip_size - bullets_in_clip) {
      painter->drawPixmap(x, y, empty_bullet_);
    } else {
      painter->drawPixmap(x, y, bullet_);
    }
    current_bullet++;
  }

  painter->restore();
}

void ReloadingField::resizeEvent(QResizeEvent*) {
  this->resize(this->size());
}
