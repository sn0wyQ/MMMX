#ifndef GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
#define GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_

#include <QVector2D>

#include "GameObject/MovableObject/movable_object.h"

class Bullet : public MovableObject {
  Q_OBJECT

 public:
  Bullet() = default;

  void OnTick() override;

 private:
  QVector2D velocity_;
  int parent_id_;
  QPointF start_point_;
  float damage_;
  float range_penalty;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
