#ifndef MODEL_GAME_DATA_MODEL_H_
#define MODEL_GAME_DATA_MODEL_H_

#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include <QDebug>

#include "GameObject/MovableObject/Entity/Player/player.h"
#include "constants.h"

class GameDataModel {
 public:
  void AddPlayer(GameObjectId player_id,
                 float x = Constants::kDefaultPlayerX,
                 float y = Constants::kDefaultPlayerY,
                 float view_angle = Constants::kDefaultPlayerViewAngle);
  void DeletePlayer(GameObjectId player_id);

  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  bool IsOwnersPlayerSet() const;
  std::shared_ptr<Player> GetOwnersPlayer() const;
  std::vector<std::shared_ptr<Player>> GetPlayers() const;
  int GetPlayersCount() const;
  bool IsPlayerIdTaken(GameObjectId player_id) const;

  GameObjectId GetOwnersPlayerId() const;
  void SetOwnersPlayerId(GameObjectId player_id);

 private:
  // Used only in ClientController
  GameObjectId owners_player_id_ = Constants::kNullGameObjectId;

  std::map<GameObjectId, std::shared_ptr<Player>> players_;
};

#endif  // MODEL_GAME_DATA_MODEL_H_
