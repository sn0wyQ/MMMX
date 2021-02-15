#ifndef ENTITY_H_
#define ENTITY_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class Entity : public GameObject {
  Q_OBJECT

 public:
  Entity() = default;

 private:
  float current_hp_;
  float base_max_hp_;
  QVector2D velocity_;
  float acceleration_;
  float max_speed_;
};

#endif  // ENTITY_H_
