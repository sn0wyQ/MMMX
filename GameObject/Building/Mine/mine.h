#ifndef MINE_H_
#define MINE_H_

#include "GameObject/Building/building.h"

class Mine : public Building {
  Q_OBJECT

 public:
  Mine() = default;

  void OnTick() override;

 private:
  int base_income_;
};

#endif  // MINE_H_
