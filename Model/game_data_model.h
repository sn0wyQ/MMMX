#ifndef MODEL_GAME_DATA_MODEL_H_
#define MODEL_GAME_DATA_MODEL_H_

#include <map>
#include <memory>
#include <sstream>
#include <utility>

#include <QDebug>

#include "Field/field.h"
#include "GameObject/Building/building.h"
#include "GameObject/MovableObject/Bullet/bullet.h"
#include "GameObject/MovableObject/Entity/Creep/creep.h"
#include "GameObject/MovableObject/Entity/Player/player.h"
#include "GameObject/Loot/loot.h"
#include "constants.h"

class GameDataModel {
 public:
  GameDataModel() = default;

  void AddPlayer(GameObjectId player_id);
  void DeletePlayer(GameObjectId player_id);

  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<Player> GetOwnersPlayer() const;
  std::shared_ptr<Player> GetTestEnemyPlayer() const;
  int GetPlayersCount() const;
  bool IsPlayerIdTaken(GameObjectId player_id) const;

  GameObjectId GetOwnersPlayerId() const;
  void SetOwnersPlayerId(GameObjectId player_id);

 private:
  // Used only in ClientController
  GameObjectId owners_player_id_ = Constants::kNullGameObjectId;

  Field map_;
  std::map<GameObjectId, std::shared_ptr<Building>> buildings_;
  std::map<GameObjectId, std::shared_ptr<Bullet>> bullets_;
  std::map<GameObjectId, std::shared_ptr<Creep>> creeps_;
  std::map<GameObjectId, std::shared_ptr<Loot>> loot_;
  std::map<GameObjectId, std::shared_ptr<Player>> players_;
};

#endif  // MODEL_GAME_DATA_MODEL_H_
