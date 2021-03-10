#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include <QVector2D>

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  Entity() = default;

  void OnTick() override = 0;

 private:
  float current_hp_ = 0.f;
  float base_max_hp_ = 0.f;
  QVector2D velocity_;
  float acceleration_ = 0.f;
  float max_speed_ = 0.f;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
