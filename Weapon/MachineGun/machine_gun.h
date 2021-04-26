#ifndef WEAPON_MACHINEGUN_MACHINE_GUN_H_
#define WEAPON_MACHINEGUN_MACHINE_GUN_H_

#include "Weapon/weapon.h"

using Constants::Weapon::MachineGun::kDefaultBulletDamage;
using Constants::Weapon::MachineGun::kDefaultBulletSpeed;
using Constants::Weapon::MachineGun::kDefaultBulletRange;
using Constants::Weapon::MachineGun::kDefaultRateOfFire;
using Constants::Weapon::MachineGun::kDefaultReloadingTime;
using Constants::Weapon::MachineGun::kDefaultClipSize;

class MachineGun : public Weapon {
 public:
  MachineGun();
  MachineGun(const MachineGun& machine_gun);

  void DrawWeapon(Painter *painter) override;
};
#endif  // WEAPON_MACHINEGUN_MACHINE_GUN_H_
