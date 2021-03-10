#ifndef GAMEOBJECT_BUILDING_STORE_STORE_H_
#define GAMEOBJECT_BUILDING_STORE_STORE_H_

#include "GameObject/Building/building.h"

class Store : public Building {
 public:
  Store() = default;

  void OnTick() override;
};

#endif  // GAMEOBJECT_BUILDING_STORE_STORE_H_
