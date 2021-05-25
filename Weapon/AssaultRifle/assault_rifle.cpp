#include "assault_rifle.h"

AssaultRifle::AssaultRifle(const AssaultRifle& assault_rifle) : Weapon(
    assault_rifle) {}

AssaultRifle::AssaultRifle() {
  this->SetParams(WeaponSettings::GetInstance().GetParams(
      WeaponType::kAssaultRifle));
}

void AssaultRifle::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
  painter->DrawRect(1.5f, -0.1f, 0.2f, 0.1f);
}

WeaponType AssaultRifle::GetWeaponType() const {
  return WeaponType::kAssaultRifle;
}

float AssaultRifle::GetBulletAngleByShift(float random_bullet_shift) const {
  float accuracy_function =
      random_bullet_shift * random_bullet_shift * GetAccuracy();
  return (random_bullet_shift < 0) ? -accuracy_function : accuracy_function;
}
