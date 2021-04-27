#include "shotgun.h"

Shotgun::Shotgun(const Shotgun& shotgun) : Weapon(shotgun) {}

Shotgun::Shotgun() {
  SetAccuracy(Constants::Weapon::Shotgun::kDefaultAccuracy);
  SetBulletDamage(Constants::Weapon::Shotgun::kDefaultBulletDamage);
  SetBulletSpeed(Constants::Weapon::Shotgun::kDefaultBulletSpeed);
  SetBulletRange(Constants::Weapon::Shotgun::kDefaultBulletRange);
  SetRateOfFire(Constants::Weapon::Shotgun::kDefaultRateOfFire);
  SetReloadingTime(Constants::Weapon::Shotgun::kDefaultReloadingTime);
  SetClipSize(Constants::Weapon::Shotgun::kDefaultClipSize);
  SetCurrentBulletsInClip(Constants::Weapon::Shotgun::kDefaultClipSize);
}

void Shotgun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.3f);
  painter->DrawRect(0.7f, 0.f, 0.8f, 0.3f);
}

WeaponType Shotgun::GetWeaponType() const {
  return WeaponType::kShotgun;
}

float Shotgun::GetRotationWithAccuracy(float random_bullet_shift) const {
  float accuracy_function = random_bullet_shift * random_bullet_shift *
      Shotgun::GetAccuracy();
  return (random_bullet_shift < 0) ? -accuracy_function : accuracy_function;
}
