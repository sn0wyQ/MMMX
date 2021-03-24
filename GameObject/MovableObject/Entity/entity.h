#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  Entity(GameObjectId id, std::shared_ptr<RigidBody> rigid_body);

  void OnTick(int time_from_previous_tick) override = 0;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
