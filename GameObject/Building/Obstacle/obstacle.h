#ifndef GAMEOBJECT_BUILDING_OBSTACLE_OBSTACLE_H_
#define GAMEOBJECT_BUILDING_OBSTACLE_OBSTACLE_H_

#include "GameObject/Building/building.h"

class Obstacle : public Building {
  Q_OBJECT

 public:
  Obstacle() = default;

  void OnTick() override;

 private:
};

#endif  // GAMEOBJECT_BUILDING_OBSTACLE_OBSTACLE_H_
