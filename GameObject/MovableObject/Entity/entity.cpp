#include "entity.h"

Entity::Entity(GameObjectId id, std::shared_ptr<RigidBody> rigid_body)
  : MovableObject(id, std::move(rigid_body)) {}

float Entity::GetViewAngle() const {
  return view_angle_;
}

void Entity::SetViewAngle(float view_angle) {
  view_angle_ = view_angle;
}
