#ifndef GAMEOBJECT_BUILDING_TURRET_TURRET_H_
#define GAMEOBJECT_BUILDING_TURRET_TURRET_H_

#include "GameObject/Building/building.h"

class Turret : public Building {
  Q_OBJECT

 public:
  Turret() = default;

  void OnTick() override;

 private:
  float current_hp_;
  float base_max_hp_;
  float base_range_;
  float base_regeneration_;
};

#endif  // GAMEOBJECT_BUILDING_TURRET_TURRET_H_
