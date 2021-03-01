#ifndef ENTITY_H_
#define ENTITY_H_

#include <QVector2D>

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
  Q_OBJECT

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

#endif  // ENTITY_H_
