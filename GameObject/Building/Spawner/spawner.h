#ifndef GAMEOBJECT_BUILDING_SPAWNER_SPAWNER_H_
#define GAMEOBJECT_BUILDING_SPAWNER_SPAWNER_H_

#include "GameObject/Building/building.h"

class Spawner : public Building {
 public:
  Spawner() = default;

  void OnTick() override;

 private:
  float cool_down_;
  float cool_down_time_left_;
  float spawn_range_;
  int count_;
};

#endif  // GAMEOBJECT_BUILDING_SPAWNER_SPAWNER_H_
