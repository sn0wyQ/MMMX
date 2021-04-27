#include "client_game_model.h"

bool ClientGameModel::IsLocalPlayerSet() const {
  return local_player_id_ != Constants::kNullGameObjectId;
}

std::shared_ptr<Player> ClientGameModel::GetLocalPlayer() const {
  if (local_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("[MODEL] Owner's player_id isn't set...");
  }
  return GameModel::GetPlayerByPlayerId(local_player_id_);
}

std::shared_ptr<PlayerStats> ClientGameModel::GetLocalPlayerStats() {
  return GameModel::GetPlayerStatsByPlayerId(local_player_id_);
}

void ClientGameModel::SetLocalPlayerId(GameObjectId player_id) {
  local_player_id_ = player_id;
  this->GetLocalPlayer()->SetIsLocalPlayer(true);
}

void ClientGameModel::AddInterpolateInfo(GameObjectId game_object_id,
                                         GameObjectType game_object_type,
                                         int64_t server_time) {
  std::shared_ptr<GameObject> game_object;
  if (this->IsGameObjectIdTaken(game_object_id)) {
    game_object =
        this->GetGameObjectByGameObjectId(game_object_id)->Clone();
  } else {
    game_object =
        this->GetNewEmptyGameObject(game_object_id, game_object_type);
  }
  game_object->SetUpdatedTime(server_time);
  interpolator_[game_object_id] = game_object;
}

std::shared_ptr<GameObject>
    ClientGameModel::GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const {
  if (interpolator_.find(game_object_id) == interpolator_.end()) {
    throw std::runtime_error("No such game object to interpolate");
  }
  return interpolator_.at(game_object_id);
}

const std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
  ClientGameModel::GetInterpolatorMap() const {
  return interpolator_;
}

bool ClientGameModel::IsGameObjectInInterpolation(
    GameObjectId game_object_id) const {
  return interpolator_.find(game_object_id) != interpolator_.end();
}

void ClientGameModel::RemoveFromInterpolator(GameObjectId game_object_id) {
  interpolator_.erase(game_object_id);
}

void ClientGameModel::AddLocalBullets(int64_t timestamp) {
  auto local_player = this->GetLocalPlayer();
  std::vector<std::vector<QVariant>> bullets = local_player->GetWeapon()->
      GetBulletsParams(local_player_id_,
                      local_player->GetX(),
                      local_player->GetY(),
                      local_player->GetRotation());

  for (const auto& bullet_params : bullets) {
    auto bullet = std::make_shared<Bullet>(bullet_id_to_set_);
    bullet->SetParams(bullet_params);
    bullet->SetUpdatedTime(timestamp);
    local_bullets_.emplace(std::make_pair(bullet_id_to_set_, bullet));
    qDebug().noquote() << "[MODEL] Added new local bullet:"
      << bullet_id_to_set_;
    bullet_id_to_set_++;
  }
}

std::vector<std::shared_ptr<Bullet>> ClientGameModel::GetLocalBullets() const {
  std::vector<std::shared_ptr<Bullet>> result;
  for (const auto& [bullet_id, bullet] : local_bullets_) {
    result.push_back(bullet);
  }
  return result;
}

void ClientGameModel::DeleteLocalBullet(GameObjectId bullet_id) {
  auto iter = local_bullets_.find(bullet_id);
  if (iter == local_bullets_.end()) {
    return;
  }
  local_bullets_.erase(iter);
  qDebug().noquote() << "[MODEL] Removed local bullet:" << bullet_id;
}
