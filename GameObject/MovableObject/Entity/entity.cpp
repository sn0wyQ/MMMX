#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

void Entity::SetParams(std::vector<QVariant> params) {
  fov_radius_ = params.back().toFloat();
  params.pop_back();
  MovableObject::SetParams(params);
}

std::vector<QVariant> Entity::GetParams() const {
  std::vector<QVariant> result = MovableObject::GetParams();
  result.emplace_back(fov_radius_);
  return result;
}

float Entity::GetFovRadius() const {
  return fov_radius_;
}

void Entity::SetFovRadius(float fov_radius) {
  fov_radius_ = fov_radius;
}
