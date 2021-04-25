#ifndef WEAPON_MACHINEGUN_MACHINE_GUN_H_
#define WEAPON_MACHINEGUN_MACHINE_GUN_H_

#include "Weapon/weapon.h"

class MachineGun : public Weapon {
 public:
  MachineGun();
  MachineGun(const MachineGun& machine_gun);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() const override;
};

#endif  // WEAPON_MACHINEGUN_MACHINE_GUN_H_
