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
  interpolater_[game_object_id] = game_object;
}

std::shared_ptr<GameObject>
    ClientGameModel::GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const {
  if (interpolater_.find(game_object_id) == interpolater_.end()) {
    throw std::runtime_error("No such game object to interpolate");
  }
  return interpolater_.at(game_object_id);
}

std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
  ClientGameModel::GetInterpolator() {
  return interpolater_;
}

void ClientGameModel::AddScheduledUpdate(
    GameObjectId game_object_id, BoolVariable bool_variable,
    const ClientGameModel::BoolUpdate& bool_update) {
  auto map = &scheduled_bool_updates_[static_cast<uint32_t>(bool_variable)];
  if (map->find(game_object_id) != map->end()) {
    auto last_update = map->at(game_object_id).back();
    auto last_update_value = last_update.value;
    if (this->IsGameObjectIdTaken(game_object_id)) {
      auto game_object = this->GetGameObjectByGameObjectId(game_object_id);
      if (game_object->GetUpdatedTime() > last_update.update_time) {
        last_update_value = GetValueAccordingVariable(game_object_id,
                                                      bool_variable);
      }
    }
    if (last_update_value == bool_update.value) {
      return;
    }
  }
  (*map)[game_object_id].push_back(bool_update);
}

void ClientGameModel::UpdateScheduledBools(int64_t current_time) {
  for (size_t i = 0; i < scheduled_bool_updates_.size(); i++) {
    auto bool_variable = static_cast<BoolVariable>(i);
    for (auto& [game_object_id, deque_bool_update]
      : scheduled_bool_updates_[i]) {
      while (!deque_bool_update.empty()) {
        BoolUpdate bool_update = deque_bool_update.front();
        if (bool_update.update_time < current_time) {
          if (this->IsGameObjectIdTaken(game_object_id)) {
            SetValueAccordingVariable(game_object_id, bool_variable,
                                      bool_update.value);
          }
          deque_bool_update.pop_front();
        } else {
          break;
        }
      }
    }
  }
}

bool ClientGameModel::GetValueAccordingVariable(
    GameObjectId game_object_id, BoolVariable bool_variable) {
  switch (bool_variable) {
    case BoolVariable::kIsInFov:
      return GetGameObjectByGameObjectId(game_object_id)->IsInFov();
    default:
      return false;
  }
}

void ClientGameModel::SetValueAccordingVariable(
    GameObjectId game_object_id, BoolVariable bool_variable, bool value) {
  switch (bool_variable) {
    case BoolVariable::kIsInFov:
      GetGameObjectByGameObjectId(game_object_id)->SetIsInFov(value);
      break;
    default:
      break;
  }
}
