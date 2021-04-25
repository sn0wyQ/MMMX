#ifndef WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_
#define WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_

#include "Weapon/weapon.h"

// using Constants::Weapon::AssaultRifle::kDefaultBaseBulletDamage;
// using Constants::Weapon::AssaultRifle::kDefaultBulletSpeed;
// using Constants::Weapon::AssaultRifle::kDefaultBulletRange;
// using Constants::Weapon::AssaultRifle::kDefaultRateOfFire;
// using Constants::Weapon::AssaultRifle::kDefaultReloadingTime;
// using Constants::Weapon::AssaultRifle::kDefaultClipSize;

using namespace Constants::Weapon::AssaultRifle;

class AssaultRifle : public Weapon {
 public:
  AssaultRifle();
  AssaultRifle(const AssaultRifle& assault_rifle);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() override;
};

#endif  // WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_
