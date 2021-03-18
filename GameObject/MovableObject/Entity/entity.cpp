#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

float Entity::GetViewAngle() const {
  return view_angle_;
}

void Entity::SetViewAngle(float view_angle) {
  view_angle_ = view_angle;
}

float Entity::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultPlayerSpeed;
}
