#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>

#include "GameObject/MovableObject/Entity/Player/active_bonus.h"
#include "GameObject/MovableObject/Entity/Player/inventory.h"
#include "GameObject/MovableObject/Entity/Player/skill.h"
#include "GameObject/MovableObject/Entity/Player/passive_ability.h"
#include "GameObject/MovableObject/Entity/Player/weapon.h"
#include "GameObject/MovableObject/Entity/entity.h"
#include "constants.h"

class Player : public Entity {
 public:
  Player() = default;
  explicit Player(GameObjectId player_id);
  ~Player() = default;

  void OnTick() override {};

  void ChangeTestCounter(int delta);
  int GetTestCounterValue() const;
  void SetTestCounterValue(int value);

  int GetTestCounterLastDeltaValue() const;
  void SetTestCounterLastDeltaValue(int value);

 private:
  int test_counter_ = 0;
  int test_counter_last_delta_ = 0;

  int experience_ = 0;
  Inventory inventory_;
  Skill skill_;
  PassiveAbility passive_ability_;
  Weapon weapon_;
  std::vector<ActiveBonus> active_bonuses_;
  int score_ = 0;
  std::vector<GameObjectId> bases_under_control_;
  float base_regeneration_ = 0;
  int kills_ = 0;
  int deaths_ = 0;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
