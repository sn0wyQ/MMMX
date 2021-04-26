#include "weapon.h"

void Weapon::SetCurrentBulletsInClip(int current_bullets_in_clip) {
  current_bullets_in_clip_ = current_bullets_in_clip;
}

bool Weapon::IsPossibleToShoot(int64_t cur_time) const {
  if (cur_time - last_time_shot_ < GetTimeBetweenShoots()
      || GetCurrentBulletsInClip() <= 0
      || cur_time - last_time_pressed_reload_ < reloading_time_) {
    return false;
  }
  return true;
}

void Weapon::Reload(int64_t cur_time) {
  last_time_pressed_reload_ = cur_time;
  SetCurrentBulletsInClip(GetClipSize());
}

int64_t Weapon::GetTimeBetweenShoots() const {
  return 60'000 / rate_of_fire_;
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

void Weapon::SetLastTimeShot(int64_t cur_time) {
  last_time_shot_ = cur_time;
}

std::vector<QVariant> Weapon::GetBulletParams(GameObjectId parent_id,
                                              float x,
                                              float y,
                                              float rotation,
                                              float radius) const {
  QVector2D velocity = Math::GetVectorByAngle(rotation);
  velocity *= this->GetBulletSpeed();
  return {x, y, 0.f, radius * 2.f, radius * 2.f,
          static_cast<int>(RigidBodyType::kCircle),
          static_cast<float>(velocity.x()),
          static_cast<float>(velocity.y()),
          parent_id, x, y,
          this->GetBulletDamage(),
          this->GetBulletSpeed(),
          this->GetBulletRange()};
}

std::vector<std::vector<QVariant>> Weapon::GetBulletsParams(
    GameObjectId parent_id,
    float x,
    float y,
    float rotation) const {
  std::vector<std::vector<QVariant>> bullets_params;
  WeaponType weapon_type = GetWeaponType();
  switch (weapon_type) {
    case WeaponType::kAssaultRifle: {
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation),
                          Constants::Weapon::AssaultRifle::kDefaultBulletRadius));
      break;
    }
    case WeaponType::kCrossbow: {
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation),
                          Constants::Weapon::Crossbow::kDefaultBulletRadius));
      break;
    }
    case WeaponType::kMachineGun: {
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation),
                          Constants::Weapon::MachineGun::kDefaultBulletRadius));
      break;
    }
    case WeaponType::kShotgun: {
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation + 10.f),
                          Constants::Weapon::Shotgun::kDefaultBulletRadius));
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation),
                          Constants::Weapon::Shotgun::kDefaultBulletRadius));
      bullets_params.emplace_back(
          GetBulletParams(parent_id,
                          x,
                          y,
                          Math::GetNormalizeAngle(rotation - 10.f),
                          Constants::Weapon::Shotgun::kDefaultBulletRadius));
      break;
    }

  }
  return bullets_params;
}
