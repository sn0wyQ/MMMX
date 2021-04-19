#include "machine_gun.h"

void MachineGun::DrawWeapon(Painter* painter) {
  painter->DrawRect(QPointF(0.7f, -0.3f),
                         QPointF(1.5f, -0.3f),
                         QPointF(1.5f, 0.3f),
                         QPointF(0.7f, 0.3f));
}
MachineGun::MachineGun(float base_bullet_damage,
                       float bullet_speed,
                       float bullet_range,
                       int rate_of_fire,
                       float base_reloading_time,
                       int base_clip_size) {
  SetBaseBulletDamage(base_bullet_damage);
  SetCurrentBulletDamage(base_bullet_damage);
  SetBulletSpeed(bullet_speed);
  SetBulletRange(bullet_range);
  SetRateOfFire(rate_of_fire);
  SetBaseReloadingTime(base_reloading_time);
  SetCurrentReloadingTime(base_reloading_time);
  SetBaseClipSize(base_clip_size);
  SetCurrentClipSize(base_clip_size);
}





