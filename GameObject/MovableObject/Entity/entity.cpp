#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

float Entity::GetViewAngle() const {
  return view_angle_;
}

void Entity::SetViewAngle(float view_angle) {
  view_angle_ = view_angle;
}

bool Entity::IsInFOV() const {
  return is_in_players_FOV;
}

void Entity::SetIsInFOV(bool is_in_players_FOV_) {
  is_in_players_FOV = is_in_players_FOV_;
}
