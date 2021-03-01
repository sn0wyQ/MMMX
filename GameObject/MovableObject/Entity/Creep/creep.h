#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_

#include "GameObject/MovableObject/Entity/entity.h"

class Creep : public Entity {
  Q_OBJECT

 public:
  Creep() = default;

  void OnTick() override {};

 private:
  int level_;
  int experience_for_kill_;
  int gold_for_kill_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
