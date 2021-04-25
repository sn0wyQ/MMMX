#include "assault_rifle.h"

AssaultRifle::AssaultRifle(const AssaultRifle& assault_rifle) : Weapon(
    assault_rifle) {}

AssaultRifle::AssaultRifle() {
  SetBulletDamage(kDefaultBaseBulletDamage);
  SetBulletSpeed(kDefaultBulletSpeed);
  SetBulletRange(kDefaultBulletRange);
  SetRateOfFire(kDefaultRateOfFire);
  SetReloadingTime(kDefaultReloadingTime);
  SetClipSize(kDefaultClipSize);
  SetCurrentBulletsInClip(kDefaultClipSize);
}

void AssaultRifle::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
  painter->DrawRect(1.5f, -0.07f, 0.2f, 0.07f);
}

WeaponType AssaultRifle::GetWeaponType() {
  return WeaponType::kAssaultRifle;
}
