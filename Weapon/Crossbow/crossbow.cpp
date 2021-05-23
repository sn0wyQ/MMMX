#include "crossbow.h"

Crossbow::Crossbow(const Crossbow& crossbow) : Weapon(crossbow) {}

Crossbow::Crossbow() {
  this->SetParams(WeaponSettings::GetInstance().GetParams(
      WeaponType::kCrossbow));
}

void Crossbow::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
  painter->DrawTriangle(QPointF(1.5f, 0.f),
                        QPointF(1.f, 0.4f),
                        QPointF(1.1f, 0.5f));
  painter->DrawTriangle(QPointF(1.5f, 0.f),
                        QPointF(1.f, -0.4f),
                        QPointF(1.1f, -0.5f));
}

WeaponType Crossbow::GetWeaponType() const {
  return WeaponType::kCrossbow;
}

float Crossbow::GetBulletAngleByShift(float random_bullet_shift) const {
  float accuracy_function = random_bullet_shift * random_bullet_shift *
      Crossbow::GetAccuracy();
  return (random_bullet_shift < 0) ? -accuracy_function : accuracy_function;
}
