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
