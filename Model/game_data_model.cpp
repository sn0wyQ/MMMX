#include "game_data_model.h"

std::shared_ptr<Player>
    GameDataModel::GetPlayerByPlayerId(GameObjectId player_id) const {
  auto iter = players_.find(player_id);
  if (iter != players_.end()) {
    return iter->second;
  }

  throw std::runtime_error("[MODEL] Trying to get invalid player...");
}

std::shared_ptr<Player> GameDataModel::GetOwnersPlayer() const {
  if (owners_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("[MODEL] Owner's player_id isn't set...");
  }

  return players_.at(owners_player_id_);
}

int GameDataModel::GetPlayersCount() const {
  return players_.size();
}

void GameDataModel::AddPlayer(GameObjectId player_id, int x, int y) {
  if (players_.find(player_id) != players_.end()) {
    throw std::runtime_error("[MODEL] This player_id already exists");
  }
  players_.emplace(std::make_pair(player_id,
                                  std::make_unique<Player>(player_id, x, y)));
  qInfo().noquote() << "[MODEL] Added new Player ID:" << player_id;
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

void GameDataModel::DeletePlayer(GameObjectId player_id) {
  auto player_to_delete = players_.find(player_id);
  if (player_to_delete != players_.end()) {
    players_.erase(player_to_delete);
  }
  qInfo().noquote() << "[MODEL] Removed Player ID:" << player_id;
}

std::vector<std::shared_ptr<Player>> GameDataModel::GetPlayers() const {
  std::vector<std::shared_ptr<Player>> result;
  for (const auto& player : players_) {
    result.push_back(player.second);
  }
  return result;
}
