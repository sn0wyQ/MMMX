#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id,
                             std::shared_ptr<Rigidbody> rigidbody)
    : GameObject(id, std::move(rigidbody)) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

void MovableObject::ApplyVelocity(int time_from_previous_tick) {
  QPointF pos = GetPosition();
  pos += GetVelocity().toPointF() * GetCurrentSpeed() * time_from_previous_tick;
  SetPosition(pos);
}

float MovableObject::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultMovableObjectSpeed;
}
