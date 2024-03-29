#include "weapon.h"

bool Weapon::IsPossibleToShoot(int64_t cur_time) const {
  if (cur_time - last_time_shot_ < GetTimeBetweenShoots()
      || GetCurrentBulletsInClip() <= 0
      || cur_time - last_time_pressed_reload_ < reloading_time_) {
    return false;
  }
  return true;
}

bool Weapon::IsPossibleToReload(int64_t cur_time) const {
  return (cur_time - last_time_pressed_reload_) >= GetReloadingTime();
}

void Weapon::Reload(int64_t cur_time) {
  last_time_pressed_reload_ = cur_time;
  SetCurrentBulletsInClip(GetClipSize());
}

int64_t Weapon::GetTimeBetweenShoots() const {
  return 60'000 / rate_of_fire_;
}

float Weapon::GetAccuracy() const {
  return accuracy_;
}

void Weapon::SetAccuracy(float accuracy) {
  accuracy_ = accuracy;
}

float Weapon::GetBulletDamage() const {
  return bullet_damage_;
}

void Weapon::SetBulletDamage(float bullet_damage) {
  bullet_damage_ = bullet_damage;
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

int64_t Weapon::GetReloadingTime() const {
  return reloading_time_;
}

void Weapon::SetReloadingTime(int64_t reloading_time) {
  reloading_time_ = reloading_time;
}

int Weapon::GetClipSize() const {
  return clip_size_;
}

void Weapon::SetClipSize(int clip_size) {
  clip_size_ = clip_size;
}

int Weapon::GetCurrentBulletsInClip() const {
  return current_bullets_in_clip_;
}

void Weapon::SetCurrentBulletsInClip(int current_bullets_in_clip) {
  current_bullets_in_clip_ = current_bullets_in_clip;
}

void Weapon::SetLastTimeShot(int64_t cur_time) {
  last_time_shot_ = cur_time;
}

int64_t Weapon::GetLastTimePressedReload() const {
  return last_time_pressed_reload_;
}

std::vector<QVariant> Weapon::GetBulletParams(GameObjectId parent_id,
                              float x, float y, float rotation,
                              float radius, float random_bullet_shift) const {
  auto angle = Math::GetNormalizeAngle(
      rotation + GetBulletAngleByShift(random_bullet_shift));
  QVector2D velocity = Math::GetVectorByAngle(angle);
  float start_x = x + velocity.x();
  float start_y = y + velocity.y();
  velocity *= this->GetBulletSpeed();
  auto animation_type = GetWeaponType() == WeaponType::kCrossbow ?
      AnimationType::kCrossbowBullet : AnimationType::kFilledBullet;
  return {start_x, start_y, Math::GetNormalizeAngle(angle - 90),
          radius * 2.f, radius * 6.f,
          static_cast<int>(RigidBodyType::kCircle),
          radius * 2.f, radius * 2.f,
          static_cast<int>(animation_type),
          static_cast<float>(velocity.x()),
          static_cast<float>(velocity.y()),
          Constants::kDefaultSpeedMultiplier,
          parent_id, start_x, start_y,
          this->GetBulletDamage(),
          this->GetBulletSpeed(),
          this->GetBulletRange()};
}

std::vector<std::vector<QVariant>> Weapon::GetBulletsParams(
    GameObjectId parent_id, float x, float y, float rotation,
    const QList<QVariant>& random_bullet_shifts) const {
  std::vector<std::vector<QVariant>> bullets_params;
  WeaponType weapon_type = GetWeaponType();
  auto bullet_radius = WeaponSettings::GetInstance().
      GetWeaponSetting<float>(weapon_type, "bullet_radius");
  switch (weapon_type) {
    case WeaponType::kAssaultRifle:
    case WeaponType::kCrossbow:
    case WeaponType::kMachineGun: {
      bullets_params.emplace_back(
          GetBulletParams(parent_id, x, y, Math::GetNormalizeAngle(rotation),
        bullet_radius, random_bullet_shifts.front().toFloat()));
      break;
    }
    case WeaponType::kShotgun: {
      auto angle_between_bullets = WeaponSettings::GetInstance().
          GetWeaponSetting<float>(weapon_type, "angle_between_bullets");
      bullets_params.emplace_back(
          GetBulletParams(parent_id, x, y, Math::GetNormalizeAngle(
                              rotation + angle_between_bullets),
        bullet_radius, random_bullet_shifts.front().toFloat()));
      bullets_params.emplace_back(
          GetBulletParams(parent_id, x, y, Math::GetNormalizeAngle(rotation),
        bullet_radius, random_bullet_shifts.front().toFloat()));
      bullets_params.emplace_back(
          GetBulletParams(parent_id, x, y, Math::GetNormalizeAngle(
              rotation - angle_between_bullets),
        bullet_radius, random_bullet_shifts.front().toFloat()));
      break;
    }
    default: {
      qWarning() << "Invalid weapon type";
      break;
    }
  }
  return bullets_params;
}

void Weapon::SetParams(std::vector<QVariant> params) {
  this->SetReloadingTime(params.back().toInt());
  params.pop_back();
  this->SetRateOfFire(params.back().toInt());
  params.pop_back();
  this->SetClipSize(params.back().toInt());
  params.pop_back();
  this->SetBulletSpeed(params.back().toFloat());
  params.pop_back();
  this->SetBulletRange(params.back().toFloat());
  params.pop_back();
  this->SetBulletDamage(params.back().toFloat());
  params.pop_back();
  this->SetAccuracy(params.back().toFloat());
  params.pop_back();
  this->SetCurrentBulletsInClip(this->GetClipSize());
}
