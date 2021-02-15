#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject/Entity/Player/active_bonus.h"
#include "GameObject/Entity/Player/inventory.h"
#include "GameObject/Entity/Player/skill.h"
#include "GameObject/Entity/Player/passive_ability.h"
#include "GameObject/Entity/Player/weapon.h"
#include "GameObject/Entity/entity.h"

class Player : public Entity {
  Q_OBJECT

 public:
  Player() = default;

 private:
  int experience_;
  Inventory inventory_;
  Skill skill_;
  PassiveAbility passive_ability_;
  Weapon weapon_;
  std::vector<ActiveBonus> active_bonuses_;
  int score_;
  std::vector<int> bases_ids_;
  float base_regeneration_;
  int kills_;
  int deaths_;
};

#endif  // PLAYER_H_
