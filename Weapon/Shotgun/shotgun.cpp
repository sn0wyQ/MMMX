#include "shotgun.h"

Shotgun::Shotgun(const Shotgun& shotgun) : Weapon(shotgun) {}

Shotgun::Shotgun() {
  this->SetParams(WeaponSettings::GetInstance().GetParams(
      WeaponType::kShotgun));
}

void Shotgun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.3f);
  painter->DrawRect(0.7f, 0.f, 0.8f, 0.3f);
}

WeaponType Shotgun::GetWeaponType() const {
  return WeaponType::kShotgun;
}

float Shotgun::GetBulletAngleByShift(float random_bullet_shift) const {
  float accuracy_function =
      random_bullet_shift * random_bullet_shift * random_bullet_shift
          * GetAccuracy();
  return accuracy_function;
}
