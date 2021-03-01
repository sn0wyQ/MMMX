#ifndef GAMEOBJECT_LOOT_ITEM_ITEM_H_
#define GAMEOBJECT_LOOT_ITEM_ITEM_H_

#include "GameObject/Loot/loot.h"

enum class ItemType {
  // MUST always stay in alphabet order

  kBandage,
  kFirstAidKit,
  kGold,
  kMedKit,
};

class Item : public Loot {
  Q_OBJECT

 public:
  Item() = default;

 private:
  int count_;
  int max_stack_;
  float usage_time_;
};

#endif  // GAMEOBJECT_LOOT_ITEM_ITEM_H_
