#include "entity.h"

Entity::Entity(GameObjectId id, std::shared_ptr<RigidBodyCircle> rigid_body)
  : MovableObject(id, std::move(rigid_body)) {}
