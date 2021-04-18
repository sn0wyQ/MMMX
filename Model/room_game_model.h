#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <vector>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  GameObjectId GetNextUnusedGameObjectId() const;

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);

  bool IsNeededToSendGameObjectData(GameObjectId game_object_id) const;
  void UpdateGameObjectHash(GameObjectId game_object_id);

 private:
  std::map<GameObjectId, QByteArray> last_object_hash_;
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
