#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

void Entity::SetParams(std::vector<QVariant> params) {
  MovableObject::SetParams(params);
}

std::vector<QVariant> Entity::GetParams() const {
  return MovableObject::GetParams();
}
