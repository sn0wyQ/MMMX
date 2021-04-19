#include "machine_gun.h"

void MachineGun::DrawWeapon(Painter* painter) {
  // painter->DrawRect(QPointF(0.7f, -0.3f),
  //                        QPointF(1.5f, -0.3f),
  //                        QPointF(1.5f, 0.3f),
  //                        QPointF(0.7f, 0.3f));
}

MachineGun::MachineGun() {
  SetBulletDamage(kDefaultMachineGunBaseBulletDamage);
  SetBulletSpeed(kDefaultMachineGunBulletSpeed);
  SetBulletRange(kDefaultMachineGunBulletRange);
  SetRateOfFire(kDefaultMachineGunRateOfFire);
  SetReloadingTime(kDefaultMachineGunBaseReloadingTime);
  SetClipSize(kDefaultMachineGunBaseClipSize);
}
