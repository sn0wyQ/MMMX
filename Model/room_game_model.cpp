#include "room_game_model.h"

GameObjectId RoomGameModel::GetNextUnusedGameObjectId() const {
  GameObjectId game_object_id = 1;
  while (GameModel::IsGameObjectIdTaken(game_object_id)) {
    game_object_id++;
  }
  return game_object_id;
}

GameObjectId RoomGameModel::AddGameObject(GameObjectType type,
                                          const std::vector<QVariant>& params) {
  GameObjectId game_object_id = this->GetNextUnusedGameObjectId();
  GameModel::AddGameObject(game_object_id, type, params);
  return game_object_id;
}
