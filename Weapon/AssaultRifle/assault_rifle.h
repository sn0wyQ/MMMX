#ifndef WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_
#define WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_

#include "Weapon/weapon.h"

class AssaultRifle : public Weapon {
 public:
  AssaultRifle();
  AssaultRifle(const AssaultRifle& assault_rifle);

  void DrawWeapon(Painter *painter) override;

  WeaponType GetWeaponType() override;
};

#endif  // WEAPON_ASSAULTRIFLE_ASSAULT_RIFLE_H_
