#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id,
                             std::shared_ptr<RigidBody> rigid_body)
    : GameObject(id, std::move(rigid_body)) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

QVector2D MovableObject::GetAppliedDeltaPosition(int time_from_previous_tick) {
  return GetVelocity() * GetCurrentSpeed()
    * static_cast<float>(time_from_previous_tick);
}

void MovableObject::ApplyVelocity(int time_from_previous_tick) {
  QPointF pos = GetPosition();
  pos += GetAppliedDeltaPosition(time_from_previous_tick).toPointF();
  SetPosition(pos);
}

float MovableObject::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultMovableObjectSpeed;
}
