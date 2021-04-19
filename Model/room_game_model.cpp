#include "room_game_model.h"

RoomGameModel::RoomGameModel(const RoomGameModel& model) : GameModel(model) {}

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
  auto object = this->GetGameObjectByGameObjectId(game_object_id);
  if (object->IsEntity()) {
    auto entity = std::dynamic_pointer_cast<Entity>(object);
    entity->SetSpawnPosition(object->GetPosition());
  }
  return game_object_id;
}
