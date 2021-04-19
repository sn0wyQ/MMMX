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

void ClientGameModel::AddScheduledUpdate(
    GameObjectId game_object_id, Variable variable,
    const ClientGameModel::UpdateVariable& update) {
  auto map = &scheduled_updates_[static_cast<uint32_t>(variable)];
  if (map->find(game_object_id) != map->end()
    && !map->at(game_object_id).empty()) {
    auto last_update = map->at(game_object_id).back();
    auto last_update_value = last_update.value;
    if (this->IsGameObjectIdTaken(game_object_id)) {
      auto game_object = this->GetGameObjectByGameObjectId(game_object_id);
      if (game_object->GetUpdatedTime() > last_update.update_time) {
        last_update_value = GetValueAccordingVariable(game_object_id,
                                                      variable);
      }
    }
    if (last_update_value == update.value) {
      return;
    }
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
          }
          deque_update.pop_front();
        } else {
          break;
        }
      }
    }
  }
}

QVariant ClientGameModel::GetValueAccordingVariable(
    GameObjectId game_object_id, Variable variable) {
  switch (variable) {
    case Variable::kIsInFov:
      return GetGameObjectByGameObjectId(game_object_id)->IsInFov();
    case Variable::kVelocity:
      return std::dynamic_pointer_cast<MovableObject>(
          GetGameObjectByGameObjectId(game_object_id))->GetVelocity();
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
    case Variable::kVelocity: {
      QVector2D vec(value.toPointF());
      std::dynamic_pointer_cast<MovableObject>(
          GetGameObjectByGameObjectId(game_object_id))->SetVelocity(vec);
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

void ClientGameModel::RemoveScheduled(GameObjectId game_object_id) {
  interpolator_.erase(game_object_id);
}
