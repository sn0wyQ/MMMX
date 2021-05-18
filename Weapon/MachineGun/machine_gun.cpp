#include "machine_gun.h"

MachineGun::MachineGun(const MachineGun& machine_gun) : Weapon(machine_gun) {}

MachineGun::MachineGun() {
  this->SetParams(WeaponSettings::GetInstance().GetParams(
      WeaponType::kMachineGun));
}

void MachineGun::DrawWeapon(Painter* painter) {
  painter->DrawRect(0.7f, -0.3f, 0.8f, 0.6f);
}

WeaponType MachineGun::GetWeaponType() const {
  return WeaponType::kMachineGun;
}

float MachineGun::GetBulletAngleByShift(float random_bullet_shift) const {
  float accuracy_function = random_bullet_shift * random_bullet_shift *
      MachineGun::GetAccuracy();
  return (random_bullet_shift < 0) ? -accuracy_function : accuracy_function;
}
