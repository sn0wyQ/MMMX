#ifndef WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
#define WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_

namespace Constants::Weapon {

constexpr float kDefaultBulletRadius = 0.2f;

// Assault Rifle

namespace AssaultRifle {

constexpr float kDefaultBaseBulletDamage = 10.f;
constexpr float kDefaultBulletSpeed = 3.f;
constexpr float kDefaultBulletRange = 20.f;
constexpr int kDefaultRateOfFire = 300;
constexpr float kDefaultReloadingTime = 7'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace AssaultRifle

// Crossbow

namespace Crossbow {

constexpr float kDefaultBaseBulletDamage = 10.f;
constexpr float kDefaultBulletSpeed = 2.5f;
constexpr float kDefaultBulletRange = 30.f;
constexpr int kDefaultRateOfFire = 200;
constexpr float kDefaultReloadingTime = 7'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace Crossbow

// MachineGun

namespace MachineGun {

constexpr float kDefaultBaseBulletDamage = 5.f;
constexpr float kDefaultBulletSpeed = 5.f;
constexpr float kDefaultBulletRange = 20.f;
constexpr int kDefaultRateOfFire = 450;
constexpr float kDefaultReloadingTime = 4'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace MachineGun

// Shotgun

namespace Shotgun {

constexpr float kDefaultBaseBulletDamage = 10.f;
constexpr float kDefaultBulletSpeed = 4.f;
constexpr float kDefaultBulletRange = 12.f;
constexpr int kDefaultRateOfFire = 50;
constexpr float kDefaultReloadingTime = 4'000.f;
constexpr int kDefaultClipSize = 12;

}  // namespace Shotgun

}  // namespace Constants::Weapon

// Assault Rifle

// namespace Constants::Weapon::AssaultRifle {
//
// constexpr float kDefaultBaseBulletDamage = 10.f;
// constexpr float kDefaultBulletSpeed = 5.f;
// constexpr float kDefaultBulletRange = 30.f;
// constexpr int kDefaultRateOfFire = 1'000;
// constexpr float kDefaultReloadingTime = 7'000.f;
// constexpr int kDefaultClipSize = 12;
//
// }  // namespace Constants::Weapon::AssaultRifle

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

// Shotgun
//
// namespace Constants::Weapon::Shotgun {
// constexpr float kDefaultBaseBulletDamage = 5.f;
// constexpr float kDefaultBulletSpeed = 10.f;
// constexpr float kDefaultBulletRange = 20.f;
// constexpr int kDefaultRateOfFire = 600;
// constexpr float kDefaultReloadingTime = 4'000.f;
// constexpr int kDefaultClipSize = 100;
//
// }  // namespace Constants::Weapon::Shotgun

#endif  // WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
