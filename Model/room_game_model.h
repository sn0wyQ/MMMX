#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <vector>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  RoomGameModel() = default;
  RoomGameModel(const RoomGameModel& model);

  GameObjectId GetNextUnusedGameObjectId();

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);
 private:
  GameObjectId next_game_object_id_{1};
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
