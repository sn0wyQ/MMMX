#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>

#include "GameObject/MovableObject/Entity/entity.h"
#include "constants.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id);
  Player(GameObjectId player_id, int x, int y);

  void OnTick() override {};

  void ApplyDirection(uint32_t mask);
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
