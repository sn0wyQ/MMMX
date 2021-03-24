#include "entity.h"

Entity::Entity(GameObjectId id, std::shared_ptr<RigidBody> rigid_body)
  : MovableObject(id, std::move(rigid_body)) {}
