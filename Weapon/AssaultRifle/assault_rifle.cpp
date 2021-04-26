#include "assault_rifle.h"

AssaultRifle::AssaultRifle(const AssaultRifle& assault_rifle) : Weapon(
    assault_rifle) {}

AssaultRifle::AssaultRifle() {
  SetBulletDamage(Constants::Weapon::AssaultRifle::kDefaultBulletDamage);
  SetBulletSpeed(Constants::Weapon::AssaultRifle::kDefaultBulletSpeed);
  SetBulletRange(Constants::Weapon::AssaultRifle::kDefaultBulletRange);
  SetRateOfFire(Constants::Weapon::AssaultRifle::kDefaultRateOfFire);
  SetReloadingTime(Constants::Weapon::AssaultRifle::kDefaultReloadingTime);
  SetClipSize(Constants::Weapon::AssaultRifle::kDefaultClipSize);
  SetCurrentBulletsInClip(Constants::Weapon::AssaultRifle::kDefaultClipSize);
}

void AssaultRifle::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
  painter->DrawRect(1.5f, -0.1f, 0.2f, 0.1f);
}

WeaponType AssaultRifle::GetWeaponType() const {
  return WeaponType::kAssaultRifle;
}
