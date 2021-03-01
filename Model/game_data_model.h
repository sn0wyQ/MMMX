#ifndef MODEL_GAME_DATA_MODEL_H_
#define MODEL_GAME_DATA_MODEL_H_

#include <memory>

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
  Player* GetPlayerByPlayerId(GameObjectId player_id);
  Player* GetOwnersPlayer();
  Player* GetTestEnemyPlayer();
  int GetPlayersCount() const;
  bool IsPlayerIdTaken(GameObjectId player_id) const;

  GameObjectId GetOwnersPlayerId() const;
  void SetOwnersPlayerId(GameObjectId player_id);

 private:
  // Used only in ClientController
  GameObjectId owners_player_id_ = Constants::kNullGameObjectId;

  Field map_;
  std::map<GameObjectId, std::unique_ptr<Building>> buildings_;
  std::map<GameObjectId, std::unique_ptr<Bullet>> bullets_;
  std::map<GameObjectId, std::unique_ptr<Creep>> creeps_;
  std::map<GameObjectId, std::unique_ptr<Loot>> loot_;
  std::map<GameObjectId, std::unique_ptr<Player>> players_;
};

#endif  // MODEL_GAME_DATA_MODEL_H_
