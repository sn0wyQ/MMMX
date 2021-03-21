#include "game_data_model.h"

#include <utility>

std::shared_ptr<Player>
    GameDataModel::GetPlayerByPlayerId(GameObjectId player_id) const {
  auto iter = players_.find(player_id);
  if (iter != players_.end()) {
    return iter->second;
  }

  throw std::runtime_error("[MODEL] Trying to get invalid player...");
}

bool GameDataModel::IsLocalPlayerSet() const {
  return local_player_id_ != Constants::kNullGameObjectId;
}

std::shared_ptr<Player> GameDataModel::GetLocalPlayer() const {
  if (local_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("[MODEL] Owner's player_id isn't set...");
  }

  return players_.at(local_player_id_);
}

int GameDataModel::GetPlayersCount() const {
  return players_.size();
}

void GameDataModel::AddPlayer(GameObjectId player_id,
                              float x,
                              float y,
                              float view_angle) {
  if (players_.find(player_id) != players_.end()) {
    throw std::runtime_error("[MODEL] This player_id already exists");
  }
  players_.emplace(std::make_pair(
      player_id, std::make_unique<Player>(player_id, x, y, view_angle)));

  qInfo().noquote() << "[MODEL] Added new Player ID:" << player_id;
}

GameObjectId GameDataModel::AddPlayer(float x, float y, float view_angle) {
  GameObjectId player_id = this->GetNextUnusedGameObjectId();
  AddPlayer(player_id, x, y, view_angle);
  return player_id;
}

GameObjectId GameDataModel::GetLocalPlayerId() const {
  return local_player_id_;
}

void GameDataModel::SetLocalPlayerId(GameObjectId player_id) {
  local_player_id_ = player_id;
  GetLocalPlayer()->SetIsLocalPlayer(true);
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

std::vector<std::shared_ptr<GameObject>> GameDataModel::GetAllGameObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& player : players_) {
    result.push_back(player.second);
  }
  for (const auto& box : boxes_) {
    result.push_back(box.second);
  }
  return result;
}

bool GameDataModel::IsGameObjectIdTaken(GameObjectId game_object_id) const {
  return players_.find(game_object_id) != players_.end()
    || boxes_.find(game_object_id) != boxes_.end();
}

GameObjectId GameDataModel::GetNextUnusedGameObjectId() const {
  GameObjectId game_object_id = 1;
  while (IsGameObjectIdTaken(game_object_id)) {
    game_object_id++;
  }
  return game_object_id;
}

GameObjectId GameDataModel::AddBox(std::shared_ptr<Box> box) {
  GameObjectId game_object_id = this->GetNextUnusedGameObjectId();
  AddBox(game_object_id, std::move(box));
  return game_object_id;
}

void GameDataModel::AddBox(GameObjectId game_object_id,
                           std::shared_ptr<Box> box) {
  boxes_[game_object_id] = std::move(box);
}

