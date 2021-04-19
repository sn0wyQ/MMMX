#include "machine_gun.h"

MachineGun::MachineGun(const MachineGun& machine_gun) : Weapon(machine_gun) {}

MachineGun::MachineGun() {
  SetBulletDamage(kDefaultBaseBulletDamage);
  SetBulletSpeed(kDefaultBulletSpeed);
  SetBulletRange(kDefaultBulletRange);
  SetRateOfFire(kDefaultRateOfFire);
  SetReloadingTime(kDefaultReloadingTime);
  SetClipSize(kDefaultClipSize);

  SetCurrentBulletsInClip(kDefaultClipSize);
}

void MachineGun::DrawWeapon(Painter* painter) {
  // painter->DrawRect(QPointF(0.7f, -0.3f),
  //                        QPointF(1.5f, -0.3f),
  //                        QPointF(1.5f, 0.3f),
  //                        QPointF(0.7f, 0.3f));
}
