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

QVariant ClientGameModel::GetScheduledVariableValue(
    GameObjectId game_object_id, Variable variable) const {
  QVariant variable_value;
  if (this->IsGameObjectIdTaken(game_object_id)) {
    variable_value = GetValueAccordingVariable(game_object_id, variable);
  }
  auto map = &scheduled_updates_[static_cast<uint32_t>(variable)];
  if (map->find(game_object_id) != map->end()
      && !map->at(game_object_id).empty()) {
    auto last_update = map->at(game_object_id).back();
    auto last_update_value = last_update.value;
    if (this->IsGameObjectIdTaken(game_object_id)) {
      auto game_object = this->GetGameObjectByGameObjectId(game_object_id);
      if (game_object->GetUpdatedTime() <= last_update.update_time) {
        variable_value = last_update_value;
      }
    }
  }
  return variable_value;
}

void ClientGameModel::AddScheduledUpdate(
    GameObjectId game_object_id, Variable variable,
    const ClientGameModel::UpdateVariable& update) {
  auto map = &scheduled_updates_[static_cast<uint32_t>(variable)];
  auto scheduled = GetScheduledVariableValue(game_object_id, variable);
  if (scheduled.isValid() && scheduled == update.value) {
    return;
  }
  (*map)[game_object_id].push_back(update);
}

void ClientGameModel::UpdateScheduled(int64_t current_time) {
  for (size_t i = 0; i < scheduled_updates_.size(); i++) {
    auto variable = static_cast<Variable>(i);
    for (auto& [game_object_id, deque_update]
      : scheduled_updates_[i]) {
      while (!deque_update.empty()) {
        UpdateVariable update = deque_update.front();
        if (update.update_time < current_time) {
          if (this->IsGameObjectIdTaken(game_object_id)) {
            SetValueAccordingVariable(game_object_id, variable,
                                      update.value);
            deque_update.pop_front();
          } else {
            break;
          }
        } else {
          break;
        }
      }
    }
  }
}

QVariant ClientGameModel::GetValueAccordingVariable(
    GameObjectId game_object_id, Variable variable) const {
  switch (variable) {
    case Variable::kIsInFov:
      return GetGameObjectByGameObjectId(game_object_id)->IsInFov();
    default:
      return false;
  }
}

void ClientGameModel::SetValueAccordingVariable(
    GameObjectId game_object_id, Variable variable,
    const QVariant& value) {
  switch (variable) {
    case Variable::kIsInFov: {
      GetGameObjectByGameObjectId(game_object_id)->SetIsInFov(value.toBool());
      break;
    }
    default:
      break;
  }
}

bool ClientGameModel::IsGameObjectInInterpolation(
    GameObjectId game_object_id) const {
  return interpolator_.find(game_object_id) != interpolator_.end();
}

void ClientGameModel::RemoveFromInterpolator(GameObjectId game_object_id) {
  interpolator_.erase(game_object_id);
}

void ClientGameModel::AddLocalBullets(const QList<QVariant>& random_bullet_shifts) {
  auto local_player = this->GetLocalPlayer();
  std::vector<std::vector<QVariant>> bullets = local_player->GetWeapon()->
      GetBulletsParams(local_player_id_,
                      local_player->GetX(),
                      local_player->GetY(),
                      local_player->GetRotation(),
                      random_bullet_shifts);

  for (const auto& bullet_params : bullets) {
    auto bullet = std::make_shared<Bullet>(bullet_id_to_set_);
    bullet->SetParams(bullet_params);
    bullet->SetIsInFov(true);
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
