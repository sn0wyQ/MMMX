#include "machine_gun.h"

MachineGun::MachineGun(const MachineGun& machine_gun) : Weapon(machine_gun) {}

MachineGun::MachineGun() {
  SetAccuracy(Constants::Weapon::MachineGun::kDefaultAccuracy);
  SetBulletDamage(Constants::Weapon::MachineGun::kDefaultBulletDamage);
  SetBulletSpeed(Constants::Weapon::MachineGun::kDefaultBulletSpeed);
  SetBulletRange(Constants::Weapon::MachineGun::kDefaultBulletRange);
  SetRateOfFire(Constants::Weapon::MachineGun::kDefaultRateOfFire);
  SetReloadingTime(Constants::Weapon::MachineGun::kDefaultReloadingTime);
  SetClipSize(Constants::Weapon::MachineGun::kDefaultClipSize);
  SetCurrentBulletsInClip(Constants::Weapon::MachineGun::kDefaultClipSize);
}

void MachineGun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
}

WeaponType MachineGun::GetWeaponType() const {
  return WeaponType::kMachineGun;
}

float MachineGun::GetRotationWithAccuracy(float random_bullet_shift) const {
  float accuracy_function = random_bullet_shift * random_bullet_shift *
      MachineGun::GetAccuracy();
  return (random_bullet_shift < 0) ? -accuracy_function : accuracy_function;
}
