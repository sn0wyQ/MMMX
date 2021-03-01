#ifndef ACTIVE_BONUS_H_
#define ACTIVE_BONUS_H_

#include <QImage>

#include "GameObject/Loot/Bonus/bonus.h"

class ActiveBonus {
 public:
  ActiveBonus() = default;

 private:
  BonusType bonus_type_;
  float max_durability_ = 0.f;
  float current_durability_ = 0.f;
  QImage image_;
};

#endif  // ACTIVE_BONUS_H_
