#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <algorithm>
#include <random>
#include <memory>
#include <unordered_map>
#include <vector>

#include <QDateTime>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  RoomGameModel() = default;
  RoomGameModel(const RoomGameModel& model);

  GameObjectId GenerateNextUnusedGameObjectId();
  GameObjectId GenerateNextUnusedBulletId(GameObjectId player_id);

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);
  bool IsNeededToSendGameObjectData(GameObjectId game_object_id) const;
  void UpdateGameObjectHashes();

  void AddPlayerStats(GameObjectId player_id, QString nickname, int level);
  bool IsNeededToSendPlayerStats(GameObjectId player_id);
  void UpdatePlayerStatsHashes();
  void ClearStats();

  QPointF GetPointToSpawn(float radius_from_object,
                          bool for_player = false) const;

 private:
  std::unordered_map<GameObjectId, GameObjectId> next_bullet_id_for_player_;
  GameObjectId next_game_object_id_{1};
  std::unordered_map<GameObjectId, QByteArray> last_object_hash_;
  std::unordered_map<GameObjectId, QByteArray> last_player_stats_hash_;
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
