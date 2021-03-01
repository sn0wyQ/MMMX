#ifndef GAMEOBJECT_LOOT_BONUS_BONUS_H_
#define GAMEOBJECT_LOOT_BONUS_BONUS_H_

#include "GameObject/Loot/loot.h"

enum class BonusType {
  // MUST always stay in alphabet order

  kArmor,
  kSpeedBoost,
};

class Bonus : public Loot {
  Q_OBJECT

 public:
  Bonus() = default;

 private:
  float base_durability_;
  BonusType bonus_type_;
};

#endif  // GAMEOBJECT_LOOT_BONUS_BONUS_H_
