#ifndef MODEL_GAME_DATA_MODEL_H_
#define MODEL_GAME_DATA_MODEL_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QDebug>

#include "GameObject/MovableObject/Entity/Player/player.h"
#include "GameObject/RectangularStaticObject/box.h"
#include "GameObject/RoundStaticObject/tree.h"
#include "constants.h"

class GameDataModel {
 public:
  void AddPlayer(GameObjectId player_id,
                 float x = Constants::kDefaultPlayerX,
                 float y = Constants::kDefaultPlayerY,
                 float rotation = Constants::kDefaultPlayerRotation);
  GameObjectId AddPlayer(float x = Constants::kDefaultPlayerX,
                 float y = Constants::kDefaultPlayerY,
                 float rotation = Constants::kDefaultPlayerRotation);
  void DeletePlayer(GameObjectId player_id);

  std::shared_ptr<Player> GetPlayerByPlayerId(GameObjectId player_id) const;
  std::shared_ptr<Player> GetLocalPlayer() const;
  std::vector<std::shared_ptr<Player>> GetPlayers() const;
  std::vector<std::shared_ptr<GameObject>> GetAllGameObjects() const;
  std::vector<std::shared_ptr<MovableObject>> GetAllMovableObjects() const;
  std::vector<std::shared_ptr<RoundStaticObject>>
    GetAllRoundStaticObjects() const;
  std::vector<std::shared_ptr<RectangularStaticObject>>
    GetAllRectangularStaticObjects() const;
  std::vector<std::shared_ptr<Box>> GetBoxes() const;
  std::vector<std::shared_ptr<Tree>> GetTrees() const;
  int GetPlayersCount() const;
  bool IsGameObjectIdTaken(GameObjectId game_object_id) const;
  GameObjectId GetNextUnusedGameObjectId() const;

  void AddBox(GameObjectId game_object_id,
                      float x, float y, float rotation,
                      float width, float height);
  GameObjectId AddBox(float x, float y, float rotation,
                      float width, float height);

  void AddTree(GameObjectId game_object_id, float x, float y, float radius);
  GameObjectId AddTree(float x, float y, float radius);

  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

 private:
  // Used only in ClientController
  GameObjectId local_player_id_ = Constants::kNullGameObjectId;

  // TODO(Everyone): need to store all objects in one container
  std::map<GameObjectId, std::shared_ptr<Player>> players_;
  std::map<GameObjectId, std::shared_ptr<Box>> boxes_;
  std::map<GameObjectId, std::shared_ptr<Tree>> trees_;
};

#endif  // MODEL_GAME_DATA_MODEL_H_
