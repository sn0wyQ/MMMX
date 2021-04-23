#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

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

 private:
  GameObjectId next_game_object_id_{1};
  std::unordered_map<GameObjectId, QByteArray> last_object_hash_;
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
