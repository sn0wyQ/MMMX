#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id) : GameObject(id) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}
