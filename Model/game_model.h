#ifndef MODEL_GAME_MODEL_H_
#define MODEL_GAME_MODEL_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QMetaEnum>
#include <QDebug>

#include "GameObject/MovableObject/Entity/Creep/creep.h"
#include "GameObject/MovableObject/Entity/Player/player.h"
#include "GameObject/map_border.h"
#include "PlayerStats/player_stats.h"
#include "Constants/constants.h"

class GameModel {
 public:
  GameModel() = default;
  virtual ~GameModel() = default;
  GameModel(const GameModel& other);
  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectId(
      GameObjectId game_object_id) const;

  std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetAllExistGameObjects() const;
  std::vector<std::shared_ptr<Bullet>> GetAllBullets() const;

  std::vector<std::shared_ptr<GameObject>> GetFilteredByFovObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetNotFilteredByFovObjects() const;

  std::vector<std::shared_ptr<Player>> GetPlayers() const;
  std::vector<std::shared_ptr<Player>> GetAlivePlayers() const;
  std::vector<std::shared_ptr<Creep>> GetCreeps() const;

  bool IsGameObjectIdTaken(GameObjectId game_object_id) const;

  void AddGameObject(GameObjectId game_object_id, GameObjectType type,
                     const std::vector<QVariant>& params);

  std::vector<std::shared_ptr<PlayerStats>> GetAllPlayersStats() const;
  std::shared_ptr<PlayerStats> GetPlayerStatsByPlayerId(GameObjectId player_id);
  void DeletePlayerStats(GameObjectId player_id);

  std::shared_ptr<GameObject>
    GetNewEmptyGameObject(GameObjectId game_object_id, GameObjectType type);
  void DeleteGameObject(GameObjectId game_object_id);
  void AttachGameObject(GameObjectId game_object_id,
                        const std::shared_ptr<GameObject>& game_object);

  bool DoesObjectCollideByMoveWithSliding(
      const std::shared_ptr<GameObject>& game_object) const;
  std::vector<std::shared_ptr<GameObject>>
    GetGameObjectsToMoveWithSliding() const;

  virtual void Clear();

 protected:
  std::unordered_map<GameObjectId, std::shared_ptr<PlayerStats>> players_stats_;

 private:
  std::map<GameObjectId, std::shared_ptr<GameObject>> game_objects_;
};

#endif  // MODEL_GAME_MODEL_H_
