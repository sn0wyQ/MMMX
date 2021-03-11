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

std::shared_ptr<Player> GameDataModel::GetTestEnemyPlayer() const {
  for (const auto& player : players_) {
    if (player.second->GetId() != owners_player_id_) {
      return player.second;
    }
  }

  throw std::runtime_error("[MODEL] No enemies found...");
}

int GameDataModel::GetPlayersCount() const {
  return players_.size();
}

void GameDataModel::AddPlayer(GameObjectId player_id) {
  if (players_.find(player_id) != players_.end()) {
    std::stringstream error_stream;
    error_stream << "[MODEL] Tried to add new Player ID:" << player_id
                 << ", but this Player already exists";
    throw std::runtime_error(error_stream.str());
  }

  players_.emplace(std::make_pair(player_id,
                                  std::make_unique<Player>(player_id)));
  qInfo().noquote().nospace() << "[MODEL] Added new Player ID:" << player_id;
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
}
