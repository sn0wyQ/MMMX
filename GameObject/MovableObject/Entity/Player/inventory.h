#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "GameObject/Loot/Item/item.h"

class Inventory {
 public:
  Inventory() = default;

 private:
  int capacity_ = 0;
  std::vector<Item> items_;
};

#endif  // INVENTORY_H_
