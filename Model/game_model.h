#ifndef MODEL_GAME_MODEL_H_
#define MODEL_GAME_MODEL_H_

#include <map>
#include <memory>
#include <utility>
#include <unordered_map>
#include <vector>

#include <QMetaEnum>
#include <QDebug>

#include "GameObject/MovableObject/Entity/Player/player.h"
#include "PlayerData/player_data.h"
#include "constants.h"

class GameModel {
 public:
  GameModel() = default;
  GameModel(const GameModel& other);
  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectId(
      GameObjectId game_object_id) const;

  std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() const;
  std::vector<std::shared_ptr<Bullet>> GetAllBullets() const;

  std::vector<std::shared_ptr<GameObject>> GetFilteredByFovObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetNotFilteredByFovObjects() const;

  std::vector<std::shared_ptr<Player>> GetPlayers() const;

  bool IsGameObjectIdTaken(GameObjectId game_object_id) const;

  void AddGameObject(GameObjectId game_object_id, GameObjectType type,
                     const std::vector<QVariant>& params);

  void AddPlayerData(GameObjectId player_id, QString nickname);
  std::shared_ptr<PlayerData> GetPlayerDataByPlayerId(GameObjectId player_id);
  std::vector<std::shared_ptr<PlayerData>> GetAllPlayersData();

  std::shared_ptr<GameObject>
    GetNewEmptyGameObject(GameObjectId game_object_id, GameObjectType type);
  void DeleteGameObject(GameObjectId game_object_id);
  void AttachGameObject(GameObjectId game_object_id,
                        const std::shared_ptr<GameObject>& game_object);

 private:
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> game_objects_;
  std::unordered_map<GameObjectId, std::shared_ptr<PlayerData>> players_data_;
};

#endif  // MODEL_GAME_MODEL_H_
