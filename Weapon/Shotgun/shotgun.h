#ifndef WEAPON_SHOTGUN_SHOTGUN_H_
#define WEAPON_SHOTGUN_SHOTGUN_H_

#include "Weapon/weapon.h"

// using Constants::Weapon::Shotgun::kDefaultBaseBulletDamage;
// using Constants::Weapon::Shotgun::kDefaultBulletSpeed;
// using Constants::Weapon::Shotgun::kDefaultBulletRange;
// using Constants::Weapon::Shotgun::kDefaultRateOfFire;
// using Constants::Weapon::Shotgun::kDefaultReloadingTime;
// using Constants::Weapon::Shotgun::kDefaultClipSize;

using namespace Constants::Weapon::Shotgun;

class Shotgun : public Weapon {
 public:
  Shotgun();
  Shotgun(const Shotgun& shotgun);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() override;
};

#endif  // WEAPON_SHOTGUN_SHOTGUN_H_
