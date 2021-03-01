#ifndef LOOT_H_
#define LOOT_H_

#include "GameObject/game_object.h"

class Loot : public GameObject {
 Q_OBJECT

 public:
  Loot() = default;

  void OnTick() override {};

 private:

};

#endif  // LOOT_H_
