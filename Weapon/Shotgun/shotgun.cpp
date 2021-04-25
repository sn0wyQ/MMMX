#include "shotgun.h"

Shotgun::Shotgun(const Shotgun& shotgun) : Weapon(shotgun) {}

Shotgun::Shotgun() {
  SetBulletDamage(Constants::Weapon::Shotgun::kDefaultBaseBulletDamage);
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

WeaponType Shotgun::GetWeaponType() {
  return WeaponType::kShotgun;
}
