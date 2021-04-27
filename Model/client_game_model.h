#ifndef MODEL_CLIENT_GAME_MODEL_H_
#define MODEL_CLIENT_GAME_MODEL_H_

#include <deque>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Model/game_model.h"

class ClientGameModel : public GameModel {
 public:
  ClientGameModel() = default;
  std::shared_ptr<Player> GetLocalPlayer() const;
  std::shared_ptr<PlayerStats> GetLocalPlayerStats();
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

  void AddInterpolateInfo(GameObjectId game_object_id,
                          GameObjectType game_object_type,
                          int64_t server_time);

  bool IsGameObjectInInterpolation(GameObjectId game_object_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const;

  const std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
    GetInterpolatorMap() const;

  void RemoveFromInterpolator(GameObjectId game_object_id);

  void AddLocalBullets(int64_t timestamp);

  std::vector<std::shared_ptr<Bullet>> GetLocalBullets() const;
  void DeleteLocalBullet(GameObjectId bullet_id);

 private:
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> interpolator_;

  std::unordered_map<GameObjectId, std::shared_ptr<Bullet>> local_bullets_;
  int bullet_id_to_set_{1};
};

#endif  // MODEL_CLIENT_GAME_MODEL_H_
