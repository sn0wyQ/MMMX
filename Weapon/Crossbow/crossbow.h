#ifndef WEAPON_CROSSBOW_CROSSBOW_H_
#define WEAPON_CROSSBOW_CROSSBOW_H_

#include "Weapon/weapon.h"

class Crossbow : public Weapon {
 public:
  Crossbow();
  Crossbow(const Crossbow& crossbow);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() const override;
  float GetRotationWithAccuracy(float random_bullet_shift) const override;
};

#endif  // WEAPON_CROSSBOW_CROSSBOW_H_
