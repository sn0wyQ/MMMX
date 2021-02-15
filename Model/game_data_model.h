#ifndef GAME_DATA_MODEL_H_
#define GAME_DATA_MODEL_H_

#include "Field/field.h"
#include "GameObject/Building/building.h"
#include "GameObject/Bullet/bullet.h"
#include "GameObject/Entity/Player/player.h"
#include "GameObject/Loot/loot.h"


class GameDataModel {
 public:
  GameDataModel() = default;

 private:
  Field map_;
  std::map<int, Building> buildings_;
  std::map<int, Bullet> bullets_;
  std::map<int, Loot> loot_;
  std::map<int, Player> players_;
};

#endif  // GAME_DATA_MODEL_H_
