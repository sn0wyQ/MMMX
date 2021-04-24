#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <unordered_map>
#include <vector>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  RoomGameModel() = default;
  RoomGameModel(const RoomGameModel& model);

  GameObjectId GetNextUnusedGameObjectId() const;

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);

  bool IsNeededToSendGameObjectData(GameObjectId game_object_id) const;
  void UpdateGameObjectHashes();
  bool IsNeededToSendPlayerData(GameObjectId player_id);
  void UpdatePlayerDataHashes();

 private:
  std::unordered_map<GameObjectId, QByteArray> last_object_hash_;
  std::unordered_map<GameObjectId, QByteArray> last_player_data_hash_;
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
