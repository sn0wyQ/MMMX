#ifndef STORE_H_
#define STORE_H_

#include "GameObject/Building/building.h"

class Store : public Building {
  Q_OBJECT

 public:
  Store() = default;

  void OnTick() override;

 private:
};

#endif  // STORE_H_
