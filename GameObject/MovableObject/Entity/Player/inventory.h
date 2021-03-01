#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_INVENTORY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_INVENTORY_H_

#include <vector>

#include "GameObject/Loot/Item/item.h"

class Inventory {
 public:
  Inventory() = default;

 private:
  int capacity_ = 0;
  std::vector<Item> items_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_INVENTORY_H_
