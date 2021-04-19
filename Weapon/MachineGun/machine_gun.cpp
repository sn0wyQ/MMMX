#include "machine_gun.h"

MachineGun::MachineGun(const MachineGun& machine_gun) : Weapon(machine_gun) {}

MachineGun::MachineGun() {
  SetBulletDamage(kDefaultMachineGunBaseBulletDamage);
  SetBulletSpeed(kDefaultMachineGunBulletSpeed);
  SetBulletRange(kDefaultMachineGunBulletRange);
  SetRateOfFire(kDefaultMachineGunRateOfFire);
  SetReloadingTime(kDefaultMachineGunBaseReloadingTime);
  SetClipSize(kDefaultMachineGunBaseClipSize);
}

void MachineGun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6);
}
