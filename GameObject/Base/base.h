#ifndef GAMEOBJECT_BASE_BASE_H_
#define GAMEOBJECT_BASE_BASE_H_

#include "GameObject/Building/Mine/mine.h"
#include "GameObject/Building/Store/store.h"
#include "GameObject/Building/Turret/turret.h"
#include "GameObject/game_object.h"

class Base : public GameObject {
 public:
  Base() = default;

  void OnTick() override {};

 private:
  int owner_id_;

  Mine mine_;
  Store store_;
  Turret turret_;
};

#endif  // GAMEOBJECT_BASE_BASE_H_
