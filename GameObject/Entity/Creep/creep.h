#ifndef CREEP_H_
#define CREEP_H_

#include "GameObject/Entity/entity.h"

class Creep : public Entity {
  Q_OBJECT

 public:
  Creep() = default;

 private:
  int level_;
  int experience_for_kill_;
  int gold_for_kill_;
};

#endif  // CREEP_H_
