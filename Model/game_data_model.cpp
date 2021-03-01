#include "game_data_model.h"

Player* GameDataModel::GetPlayerByPlayerId(GameObjectId player_id) {
  return players_.at(player_id).get();
}

Player* GameDataModel::GetOwnersPlayer() {
  if (owners_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("Owner's player_id isn't set...");
  }

  return players_.at(owners_player_id_).get();
}

Player* GameDataModel::GetTestEnemyPlayer() {
  for (auto iter = players_.begin(); iter != players_.end(); iter++) {
    if (iter->second->GetId() != owners_player_id_) {
      return iter->second.get();
    }
  }

  throw std::runtime_error("No enemies...");
  return nullptr;
}

int GameDataModel::GetPlayersCount() const {
  return players_.size();
}

void GameDataModel::AddPlayer(GameObjectId player_id) {
  if (players_.find(player_id) != players_.end()) {
    qInfo() << "[MODEL] Tried to add new Player ID:" << player_id
            << ", but this Player already exists";
    return;
  }

  players_.emplace(std::make_pair(player_id,
                                  std::make_unique<Player>(player_id)));
  qInfo() << "[MODEL] Added new Player ID:" << player_id;
}

bool GameDataModel::IsPlayerIdTaken(GameObjectId player_id) const {
  return players_.find(player_id) != players_.end();
}

GameObjectId GameDataModel::GetOwnersPlayerId() const {
  return owners_player_id_;
}

void GameDataModel::SetOwnersPlayerId(GameObjectId player_id) {
  owners_player_id_ = player_id;
}
