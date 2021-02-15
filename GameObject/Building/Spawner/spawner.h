#ifndef SPAWNER_H_
#define SPAWNER_H_

#include "GameObject/Building/building.h"

class Spawner : public Building {
  Q_OBJECT

 public:
  Spawner() = default;

  void OnTick() override;

 private:
  float cool_down_;
  float cool_down_time_left_;
  float spawn_range_;
  int count_;
};

#endif  // SPAWNER_H_
