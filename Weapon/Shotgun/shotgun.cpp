#include "shotgun.h"

Shotgun::Shotgun(const Shotgun& shotgun) : Weapon(shotgun) {}

Shotgun::Shotgun() {
  SetBulletDamage(kDefaultBaseBulletDamage);
  SetBulletSpeed(kDefaultBulletSpeed);
  SetBulletRange(kDefaultBulletRange);
  SetRateOfFire(kDefaultRateOfFire);
  SetReloadingTime(kDefaultReloadingTime);
  SetClipSize(kDefaultClipSize);
  SetCurrentBulletsInClip(kDefaultClipSize);
}

void Shotgun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.3f);
  painter->DrawRect(0.7f, 0.f, 0.8f, 0.3f);
}

WeaponType Shotgun::GetWeaponType() {
  return WeaponType::kShotgun;
}
