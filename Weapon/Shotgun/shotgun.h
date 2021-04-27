#ifndef WEAPON_SHOTGUN_SHOTGUN_H_
#define WEAPON_SHOTGUN_SHOTGUN_H_

#include "Weapon/weapon.h"

class Shotgun : public Weapon {
 public:
  Shotgun();
  Shotgun(const Shotgun& shotgun);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() const override;
  float GetRotationWithAccuracy(float random_bullet_shift) const override;
};

#endif  // WEAPON_SHOTGUN_SHOTGUN_H_
