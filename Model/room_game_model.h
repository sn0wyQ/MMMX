#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  RoomGameModel() = default;
  RoomGameModel(const RoomGameModel& model);

  GameObjectId GenerateNextUnusedGameObjectId();

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);
  bool IsNeededToSendGameObjectData(GameObjectId game_object_id) const;
  void UpdateGameObjectHashes();

  void AddPlayerStats(GameObjectId player_id, QString nickname, int level);
  std::vector<std::shared_ptr<PlayerStats>> GetAllPlayersStats();
  bool IsNeededToSendPlayerStats(GameObjectId player_id);
  void UpdatePlayerStatsHashes();

 private:
  GameObjectId next_game_object_id_{1};
  std::unordered_map<GameObjectId, QByteArray> last_object_hash_;
  std::unordered_map<GameObjectId, QByteArray> last_player_stats_hash_;
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
