#include <utility>

#include "Hash/hash_calculator.h"
#include "room_game_model.h"

RoomGameModel::RoomGameModel(const RoomGameModel& model) : GameModel(model) {
  next_game_object_id_ = model.next_game_object_id_;
  last_object_hash_ = model.last_object_hash_;
  last_player_stats_hash_ = model.last_player_stats_hash_;
}

GameObjectId RoomGameModel::GenerateNextUnusedGameObjectId() {
  return next_game_object_id_++;
}

GameObjectId RoomGameModel::AddGameObject(GameObjectType type,
                                          const std::vector<QVariant>& params) {
  GameObjectId game_object_id = this->GenerateNextUnusedGameObjectId();
  GameModel::AddGameObject(game_object_id, type, params);
  auto object = this->GetGameObjectByGameObjectId(game_object_id);
  if (object->IsEntity()) {
    auto entity = std::dynamic_pointer_cast<Entity>(object);
    entity->SetSpawnPosition(object->GetPosition());
  }
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

void RoomGameModel::AddPlayerStats(GameObjectId player_id, QString nickname,
                               int level) {
  players_stats_[player_id] = std::make_shared<PlayerStats>(
      PlayerStats(player_id, std::move(nickname), level));
}

bool RoomGameModel::IsNeededToSendPlayerStats(GameObjectId player_id) {
  auto stats_iter = last_player_stats_hash_.find(player_id);
  if (stats_iter == last_player_stats_hash_.end()) {
    return true;
  }
  auto hash = HashCalculator::GetHash(
      this->GetPlayerStatsByPlayerId(player_id)->GetParams());
  return hash != stats_iter->second;
}

void RoomGameModel::UpdatePlayerStatsHashes() {
  for (const auto& stats : this->GetAllPlayersStats()) {
    last_player_stats_hash_[stats->GetPlayerId()] =
        HashCalculator::GetHash(stats->GetParams());
  }
}
