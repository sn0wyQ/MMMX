#ifndef WEAPON_CROSSBOW_CROSSBOW_H_
#define WEAPON_CROSSBOW_CROSSBOW_H_

#include "Weapon/weapon.h"

class Crossbow : public Weapon {
 public:
  Crossbow();
  Crossbow(const Crossbow& crossbow);
  virtual ~Crossbow() = default;

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() const override;
};

#endif  // WEAPON_CROSSBOW_CROSSBOW_H_
