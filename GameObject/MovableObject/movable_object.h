#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  MovableObject() = default;

  void OnTick() override = 0;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
