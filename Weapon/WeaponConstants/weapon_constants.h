#ifndef WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
#define WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_

// Assault Rifle

namespace Constants::Weapon::AssaultRifle {

constexpr float kDefaultAccuracy = 5.f;
constexpr float kDefaultBulletDamage = 12.f;
constexpr float kDefaultBulletRadius = 0.25f;
constexpr float kDefaultBulletRange = 20.f;
constexpr float kDefaultBulletSpeed = 3.f;
constexpr int kDefaultClipSize = 30;
constexpr int kDefaultRateOfFire = 300;
constexpr float kDefaultReloadingTime = 5'000.f;

}  // namespace Constants::Weapon::AssaultRifle

// Crossbow

namespace Constants::Weapon::Crossbow {

constexpr float kDefaultAccuracy = 2.f;
constexpr float kDefaultBulletDamage = 15.f;
constexpr float kDefaultBulletRadius = 0.3f;
constexpr float kDefaultBulletRange = 30.f;
constexpr float kDefaultBulletSpeed = 2.5f;
constexpr int kDefaultClipSize = 7;
constexpr int kDefaultRateOfFire = 200;
constexpr float kDefaultReloadingTime = 7'000.f;

}  // namespace Constants::Weapon::Crossbow

// Machine Gun

namespace Constants::Weapon::MachineGun {

constexpr float kDefaultAccuracy = 10.f;
constexpr float kDefaultBulletDamage = 6.f;
constexpr float kDefaultBulletRadius = 0.15f;
constexpr float kDefaultBulletRange = 20.f;
constexpr float kDefaultBulletSpeed = 5.f;
constexpr int kDefaultClipSize = 12;
constexpr int kDefaultRateOfFire = 450;
constexpr float kDefaultReloadingTime = 9'000.f;

}  // namespace Constants::Weapon::MachineGun

// Shotgun

namespace Constants::Weapon::Shotgun {

constexpr float kDefaultAccuracy = 7.f;
constexpr float kDefaultAngleBetweenBullets = 10.f;
constexpr float kDefaultBulletDamage = 8.f;
constexpr float kDefaultBulletRadius = 0.2f;
constexpr float kDefaultBulletRange = 15.f;
constexpr float kDefaultBulletSpeed = 4.f;
constexpr int kDefaultClipSize = 3 * 4;  // clip_size = 3 * x
constexpr int kDefaultRateOfFire = 90;
constexpr float kDefaultReloadingTime = 7'000.f;

}  // namespace Constants::Weapon::Shotgun

#endif  // WEAPON_WEAPONCONSTANTS_WEAPON_CONSTANTS_H_
