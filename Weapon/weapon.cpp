#include "weapon.h"

float Weapon::GetBaseBulletDamage() const {
  return base_bullet_damage_;
}

void Weapon::SetBaseBulletDamage(float base_bullet_damage) {
  base_bullet_damage_ = base_bullet_damage;
}

float Weapon::GetCurrentBulletDamage() const {
  return current_bullet_damage_;
}

void Weapon::SetCurrentBulletDamage(float current_bullet_damage) {
  current_bullet_damage_ = current_bullet_damage;
}

float Weapon::GetBulletSpeed() const {
  return bullet_speed_;
}

void Weapon::SetBulletSpeed(float bullet_speed) {
  bullet_speed_ = bullet_speed;
}

float Weapon::GetBulletRange() const {
  return bullet_range_;
}

void Weapon::SetBulletRange(float bullet_range) {
  bullet_range_ = bullet_range;
}

int Weapon::GetRateOfFire() const {
  return rate_of_fire_;
}

void Weapon::SetRateOfFire(int rate_of_fire) {
  rate_of_fire_ = rate_of_fire;
}

float Weapon::GetBaseReloadingTime() const {
  return base_reloading_time_;
}

void Weapon::SetBaseReloadingTime(float base_reloading_time) {
  base_reloading_time_ = base_reloading_time;
}

float Weapon::GetCurrentReloadingTime() const {
  return current_reloading_time_;
}

void Weapon::SetCurrentReloadingTime(float current_reloading_time) {
  current_reloading_time_ = current_reloading_time;
}

int Weapon::GetBaseClipSize() const {
  return base_clip_size_;
}

void Weapon::SetBaseClipSize(int base_clip_size) {
  base_clip_size_ = base_clip_size;
}

int Weapon::GetCurrentClipSize() const {
  return current_clip_size_;
}

void Weapon::SetCurrentClipSize(int current_clip_size) {
  current_clip_size_ = current_clip_size;
}

float Weapon::GetCurrentTimeToShoot() const {
  return current_time_to_shoot_;
}

void Weapon::SetCurrentTimeToShoot(float current_time_to_shoot) {
  current_time_to_shoot_ = current_time_to_shoot;
}
bool Weapon::GetIsReloadingNow() const {
  return is_reloading_now_;
}

void Weapon::SetIsReloadingNow(bool is_reloading_now) {
  is_reloading_now_ = is_reloading_now;
}


bool Weapon::IsPossibleToShoot() {
  if (GetCurrentTimeToShoot() > 0.f
      || GetCurrentClipSize() <= 0
      || GetIsReloadingNow()) {
    return false;
  }
  return true;
}

void Weapon::CreateBulletAndAddToModel() {
  // Bullet new_bullet = Bullet(bullet_id, parent_id, start_position);

}

void Weapon::Shoot() {
  if (IsPossibleToShoot()) {
    QTimer::singleShot(60'000 / GetRateOfFire(), this, &Weapon::Shoot);

    CreateBulletAndAddToModel();
    SetCurrentClipSize(GetCurrentClipSize() - 1);
  }
}

void Weapon::Reload() {

  SetCurrentClipSize(GetBaseClipSize());
}







