#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id,
                             std::shared_ptr<RigidBodyCircle> rigid_body)
    : GameObject(id), rigid_body_(std::move(rigid_body)) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

QVector2D MovableObject::GetVelocityByDeltaPosition(
    QVector2D position, int time_from_previous_tick) const {
  if (time_from_previous_tick == 0) {
    return QVector2D(0.f, 0.f);
  }
  return position / GetCurrentSpeed()
  / static_cast<float>(time_from_previous_tick);
}

QVector2D MovableObject::GetAppliedDeltaPosition(
    int time_from_previous_tick) const {
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

std::shared_ptr<RigidBodyCircle> MovableObject::GetRigidBody() const {
  return rigid_body_;
}

void MovableObject::OnTick(int time_from_previous_tick) {
  ApplyVelocity(time_from_previous_tick);
}
