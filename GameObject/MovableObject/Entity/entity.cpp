#include "entity.h"

Entity::Entity(GameObjectId id, std::shared_ptr<Rigidbody> rigidbody)
  : MovableObject(id, std::move(rigidbody)) {}

float Entity::GetViewAngle() const {
  return view_angle_;
}

void Entity::SetViewAngle(float view_angle) {
  view_angle_ = view_angle;
}
