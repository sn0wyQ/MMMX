#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include "GameObject/Building/building.h"

class Obstacle : public Building {
  Q_OBJECT

 public:
  Obstacle() = default;

  void OnTick() override;

 private:
};

#endif  // OBSTACLE_H_
