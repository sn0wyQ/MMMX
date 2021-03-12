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

  void ChangeTestCounter(int delta);
  int GetTestCounterValue() const;
  void SetTestCounterValue(int value);

  int GetTestCounterLastDeltaValue() const;
  void SetTestCounterLastDeltaValue(int value);

  void ApplyDirection(uint32_t mask);

 private:
  int test_counter_ = 0;
  int test_counter_last_delta_ = 0;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
