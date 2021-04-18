#include "Hash/hash_calculator.h"
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

bool RoomGameModel::IsNeededToSendGameObjectData(
    GameObjectId game_object_id) const {
  auto object_iter = last_object_hash_.find(game_object_id);
  if (object_iter == last_object_hash_.end()) {
    return true;
  }
  auto hash = HashCalculator::GetHash(
      this->GetGameObjectByGameObjectId(game_object_id)->GetParams());
  return hash != object_iter->second;
}

void RoomGameModel::UpdateGameObjectHash(GameObjectId game_object_id) {
  last_object_hash_[game_object_id] = HashCalculator::GetHash(
      this->GetGameObjectByGameObjectId(game_object_id)->GetParams());
}
