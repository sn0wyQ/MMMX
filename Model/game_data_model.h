#ifndef MODEL_GAME_DATA_MODEL_H_
#define MODEL_GAME_DATA_MODEL_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QMetaEnum>
#include <QDebug>

#include "GameObject/MovableObject/Entity/Player/player.h"
#include "constants.h"

class GameDataModel {
 public:
  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectId(
      GameObjectId game_object_id) const;
  std::shared_ptr<Player> GetLocalPlayer() const;

  std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetConstantObjects() const;
  std::vector<std::shared_ptr<MovableObject>> GetMovableObjects() const;

  std::vector<std::shared_ptr<GameObject>> GetFilteredByFovObjects() const;
  std::vector<std::shared_ptr<GameObject>> GetNotFilteredByFovObjects() const;

  std::vector<std::shared_ptr<Player>> GetPlayers() const;

  int GetPlayersCount() const;
  bool IsGameObjectIdTaken(GameObjectId game_object_id) const;
  GameObjectId GetNextUnusedGameObjectId() const;

  void AddGameObject(GameObjectId game_object_id, GameObjectType type,
                     const std::vector<QVariant>& params);
  GameObjectId AddGameObject(GameObjectType type,
                     const std::vector<QVariant>& params);
  void DeleteGameObject(GameObjectId game_object_id);

  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

 private:
  // Used only in ClientController
  GameObjectId local_player_id_ = Constants::kNullGameObjectId;

  std::map<GameObjectId, std::shared_ptr<GameObject>> game_objects_;
};

#endif  // MODEL_GAME_DATA_MODEL_H_
