#include "machine_gun.h"

MachineGun::MachineGun(const MachineGun& machine_gun) : Weapon(machine_gun) {}

MachineGun::MachineGun() {
  SetBulletDamage(Constants::Weapon::MachineGun::kDefaultBaseBulletDamage);
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

WeaponType MachineGun::GetWeaponType() {
  return WeaponType::kMachineGun;
}
