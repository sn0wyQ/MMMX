#ifndef MODEL_GAME_MODEL_H_
#define MODEL_GAME_MODEL_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QMetaEnum>
#include <QDebug>

#include "GameObject/MovableObject/Entity/Player/player.h"
#include "constants.h"

class GameModel {
 public:
  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectId(
      GameObjectId game_object_id) const;

  std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() const;

  std::vector<std::shared_ptr<GameObject>> GetFilteredByFovObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetNotFilteredByFovObjects() const;

  std::vector<std::shared_ptr<Player>> GetPlayers() const;

  bool IsGameObjectIdTaken(GameObjectId game_object_id) const;

  void AddGameObject(GameObjectId game_object_id, GameObjectType type,
                     const std::vector<QVariant>& params);

  std::shared_ptr<GameObject>
    GetNewEmptyGameObject(GameObjectId game_object_id, GameObjectType type);
  void DeleteGameObject(GameObjectId game_object_id);
  void AttachGameObject(GameObjectId game_object_id,
                        const std::shared_ptr<GameObject>& game_object);

 private:
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> game_objects_;
};

#endif  // MODEL_GAME_MODEL_H_
