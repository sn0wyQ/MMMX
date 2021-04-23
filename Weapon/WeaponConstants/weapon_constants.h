#ifndef WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
#define WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_

namespace Constants::Weapon {

constexpr float kDefaultBulletRadius = 0.5f;

}  // namespace Constants::Weapon

// Machine Gun

namespace Constants::Weapon::MachineGun {

constexpr float kDefaultBaseBulletDamage = 5.f;
constexpr float kDefaultBulletSpeed = 2.f;
constexpr float kDefaultBulletRange = 20.f;
constexpr int kDefaultRateOfFire = 360;
constexpr float kDefaultReloadingTime = 5.f;
constexpr int kDefaultClipSize = 5;

}  // namespace Constants::Weapon::MachineGun

#endif  // WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_