#ifndef BUILDING_H_
#define BUILDING_H_

#include "GameObject/game_object.h"

class Building : public GameObject {
  Q_OBJECT

 public:
  void OnTick() override {};

 private:
  int level_;
};

#endif  // BUILDING_H_
