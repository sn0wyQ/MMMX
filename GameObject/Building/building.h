#ifndef GAMEOBJECT_BUILDING_BUILDING_H_
#define GAMEOBJECT_BUILDING_BUILDING_H_

#include "GameObject/game_object.h"

class Building : public GameObject {
  Q_OBJECT

 public:
  void OnTick() override {};

 private:
  int level_;
};

#endif  // GAMEOBJECT_BUILDING_BUILDING_H_
