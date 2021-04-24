#include "crossbow.h"

Crossbow::Crossbow(const Crossbow& crossbow) : Weapon(crossbow) {}

Crossbow::Crossbow() {
  SetBulletDamage(kDefaultBaseBulletDamage);
  SetBulletSpeed(kDefaultBulletSpeed);
  SetBulletRange(kDefaultBulletRange);
  SetRateOfFire(kDefaultRateOfFire);
  SetReloadingTime(kDefaultReloadingTime);
  SetClipSize(kDefaultClipSize);
  SetCurrentBulletsInClip(kDefaultClipSize);
}

void Crossbow::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
  painter->DrawTriangle(QPointF(1.5f, 0.f),
                        QPointF(1.f, 0.1f),
                        QPointF(1.1f, 0.2f));
  painter->DrawTriangle(QPointF(1.5f, 0.f),
                        QPointF(1.f, -0.1f),
                        QPointF(1.1f, -0.2f));
}