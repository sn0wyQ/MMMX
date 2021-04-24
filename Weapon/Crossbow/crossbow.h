#ifndef WEAPON_CROSSBOW_CROSSBOW_H_
#define WEAPON_CROSSBOW_CROSSBOW_H_

#include "Weapon/weapon.h"

// using Constants::Weapon::Crossbow::kDefaultBaseBulletDamage;
// using Constants::Weapon::Crossbow::kDefaultBulletSpeed;
// using Constants::Weapon::Crossbow::kDefaultBulletRange;
// using Constants::Weapon::Crossbow::kDefaultRateOfFire;
// using Constants::Weapon::Crossbow::kDefaultReloadingTime;
// using Constants::Weapon::Crossbow::kDefaultClipSize;

using namespace Constants::Weapon::Crossbow;

class Crossbow : public Weapon {
 public:
  Crossbow();
  Crossbow(const Crossbow& crossbow);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() override;

};
#endif  // WEAPON_CROSSBOW_CROSSBOW_H_
