#include "Hash/hash_calculator.h"
#include "room_game_model.h"

RoomGameModel::RoomGameModel(const RoomGameModel& model) : GameModel(model) {
  last_object_hash_ = model.last_object_hash_;
  last_player_data_hash_ = model.last_player_data_hash_;
}

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

void RoomGameModel::UpdateGameObjectHashes() {
  for (const auto& object : this->GetAllGameObjects()) {
    last_object_hash_[object->GetId()] =
        HashCalculator::GetHash(object->GetParams());
  }
}

bool RoomGameModel::IsNeededToSendPlayerData(GameObjectId player_id) {
  auto data_iter = last_player_data_hash_.find(player_id);
  if (data_iter == last_player_data_hash_.end()) {
    return true;
  }
  auto hash = HashCalculator::GetHash(
      this->GetPlayerDataByPlayerId(player_id)->GetParams());
  return hash != data_iter->second;
}
void RoomGameModel::UpdatePlayerDataHashes() {
  for (const auto& data : this->GetAllPlayersData()) {
    last_player_data_hash_[data->GetPlayerId()] =
        HashCalculator::GetHash(data->GetParams());
  }
}
