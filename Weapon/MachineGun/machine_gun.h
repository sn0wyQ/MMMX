#ifndef WEAPON_MACHINEGUN_MACHINE_GUN_H_
#define WEAPON_MACHINEGUN_MACHINE_GUN_H_

#include "Weapon/weapon.h"

class MachineGun : public Weapon {
 public:
  MachineGun();
  MachineGun(const MachineGun& machine_gun);
  virtual ~MachineGun() = default;

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() const override;
  float GetBulletAngleByShift(float random_bullet_shift) const override;
};

#endif  // WEAPON_MACHINEGUN_MACHINE_GUN_H_
