#ifndef GAMEOBJECT_LOOT_LOOT_H_
#define GAMEOBJECT_LOOT_LOOT_H_

#include "GameObject/game_object.h"

class Loot : public GameObject {
  Q_OBJECT

 public:
  Loot() = default;

  void OnTick() override {};

 private:
};

#endif  // GAMEOBJECT_LOOT_LOOT_H_
