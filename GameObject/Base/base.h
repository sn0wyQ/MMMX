#ifndef BASE_H_
#define BASE_H_

#include "GameObject/Building/Mine/mine.h"
#include "GameObject/Building/Store/store.h"
#include "GameObject/Building/Turret/turret.h"
#include "GameObject/game_object.h"

class Base : public GameObject {
  Q_OBJECT

 public:
  Base() = default;

 private:
  int owner_id_;

  Mine mine_;
  Store store_;
  Turret turret_;
};

#endif  // BASE_H_
