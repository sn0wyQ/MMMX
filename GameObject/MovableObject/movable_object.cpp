#include "movable_object.h"

MovableObject::MovableObject(GameObjectId id,
                             std::shared_ptr<RigidBodyCircle> rigid_body)
    : GameObject(id, std::move(rigid_body)) {}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

QVector2D MovableObject::GetVelocityByDeltaPosition(
    QVector2D position, int delta_time) const {
  if (delta_time == 0) {
    return QVector2D(0.f, 0.f);
  }
  return position / GetCurrentSpeed()
  / static_cast<float>(delta_time);
}

QVector2D MovableObject::GetAppliedDeltaPosition(
    int delta_time) const {
  return GetVelocity() * GetCurrentSpeed()
    * static_cast<float>(delta_time);
}

void MovableObject::ApplyVelocity(int delta_time) {
  QPointF pos = GetPosition();
  pos += GetAppliedDeltaPosition(delta_time).toPointF();
  SetPosition(pos);
}

float MovableObject::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultMovableObjectSpeed;
}

void MovableObject::OnTick(int delta_time) {
  ApplyVelocity(delta_time);
}

bool MovableObject::IsMovable() const {
  return true;
}
