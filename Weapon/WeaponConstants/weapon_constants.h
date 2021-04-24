#ifndef WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
#define WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_

namespace Constants::Weapon {

constexpr float kDefaultBulletRadius = 0.5f;

// MachineGun

namespace MachineGun {

constexpr float kDefaultBaseBulletDamage = 5.f;
constexpr float kDefaultBulletSpeed = 5.f;
constexpr float kDefaultBulletRange = 20.f;
constexpr int kDefaultRateOfFire = 600;
constexpr float kDefaultReloadingTime = 4'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace MachineGun

// Crossbow

namespace Crossbow {

constexpr float kDefaultBaseBulletDamage = 10.f;
constexpr float kDefaultBulletSpeed = 2.5f;
constexpr float kDefaultBulletRange = 30.f;
constexpr int kDefaultRateOfFire = 1'000;
constexpr float kDefaultReloadingTime = 7'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace Crossbow

}  // namespace Constants::Weapon

// Machine Gun
//
// namespace Constants::Weapon::MachineGun {
// constexpr float kDefaultBaseBulletDamage = 5.f;
// constexpr float kDefaultBulletSpeed = 10.f;
// constexpr float kDefaultBulletRange = 20.f;
// constexpr int kDefaultRateOfFire = 600;
// constexpr float kDefaultReloadingTime = 4'000.f;
// constexpr int kDefaultClipSize = 100;
//
// }  // namespace Constants::Weapon::MachineGun

// Crossbow

// namespace Constants::Weapon::Crossbow {
//
// constexpr float kDefaultBaseBulletDamage = 10.f;
// constexpr float kDefaultBulletSpeed = 5.f;
// constexpr float kDefaultBulletRange = 30.f;
// constexpr int kDefaultRateOfFire = 1'000;
// constexpr float kDefaultReloadingTime = 7'000.f;
// constexpr int kDefaultClipSize = 12;
//
// }  // namespace Constants::Weapon::Crossbow

#endif  // WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
