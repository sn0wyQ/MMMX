#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id) : GameObject(id) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

void MovableObject::ApplyVelocity(int delta_time) {
  QPointF pos = GetPosition();
  pos += GetVelocity().toPointF() * GetCurrentSpeed() * delta_time;
  SetPosition(pos);
}

float MovableObject::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultMovableObjectSpeed;
}
