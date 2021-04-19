#ifndef WEAPON_MACHINE_GUN_MACHINE_GUN_H_
#define WEAPON_MACHINE_GUN_MACHINE_GUN_H_

#include "Weapon/weapon.h"

using Constants::Weapon::MachineGun::kDefaultMachineGunBaseBulletDamage;
using Constants::Weapon::MachineGun::kDefaultMachineGunBulletSpeed;
using Constants::Weapon::MachineGun::kDefaultMachineGunBulletRange;
using Constants::Weapon::MachineGun::kDefaultMachineGunRateOfFire;
using Constants::Weapon::MachineGun::kDefaultMachineGunBaseReloadingTime;
using Constants::Weapon::MachineGun::kDefaultMachineGunBaseClipSize;

class MachineGun : public Weapon {
 public:
  explicit MachineGun(float base_bullet_damage = kDefaultMachineGunBaseBulletDamage,
             float bullet_speed = kDefaultMachineGunBulletSpeed,
             float bullet_range = kDefaultMachineGunBulletRange,
             int rate_of_fire = kDefaultMachineGunRateOfFire,
             float base_reloading_time = kDefaultMachineGunBaseReloadingTime,
             int base_clip_size = kDefaultMachineGunBaseClipSize
             );

  void DrawWeapon(Painter *painter) override;

 private:

};
#endif  // WEAPON_MACHINE_GUN_MACHINE_GUN_H_
