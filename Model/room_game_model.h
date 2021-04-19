#ifndef MODEL_ROOM_GAME_MODEL_H_
#define MODEL_ROOM_GAME_MODEL_H_

#include <vector>

#include "Model/game_model.h"

class RoomGameModel : public GameModel {
 public:
  RoomGameModel() = default;
  RoomGameModel(const RoomGameModel& model);

  GameObjectId GetNextUnusedGameObjectId() const;

  GameObjectId AddGameObject(GameObjectType type,
                             const std::vector<QVariant>& params);
};

#endif  // MODEL_ROOM_GAME_MODEL_H_
