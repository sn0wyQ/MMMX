#include "game_data_model.h"

std::shared_ptr<Player>
    GameDataModel::GetPlayerByPlayerId(GameObjectId player_id) const {
  auto iter = game_objects_.find(player_id);
  if (iter != game_objects_.end()
    && iter->second->GetType() == GameObjectType::kPlayer) {
    return std::dynamic_pointer_cast<Player>(iter->second);
  }

  throw std::runtime_error("[MODEL] Trying to get invalid player...");
}

std::shared_ptr<GameObject> GameDataModel::GetGameObjectByGameObjectId(
    GameObjectId game_object_id) const {
  auto iter = game_objects_.find(game_object_id);
  if (iter != game_objects_.end()) {
    return iter->second;
  }

  throw std::runtime_error("[MODEL] Trying to get invalid game object...");
}

bool GameDataModel::IsLocalPlayerSet() const {
  return local_player_id_ != Constants::kNullGameObjectId;
}

std::shared_ptr<Player> GameDataModel::GetLocalPlayer() const {
  if (local_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("[MODEL] Owner's player_id isn't set...");
  }

  return GetPlayerByPlayerId(local_player_id_);
}

void GameDataModel::AddGameObject(GameObjectId game_object_id,
                                  GameObjectType type,
                                  const std::vector<QVariant>& params) {
  if (game_objects_.find(game_object_id) != game_objects_.end()) {
    throw std::runtime_error("[MODEL] This game_object_id already exists");
  }
  std::shared_ptr<GameObject> object;
  switch (type) {
    case GameObjectType::kPlayer:
      object = std::make_unique<Player>(game_object_id);
      break;
    case GameObjectType::kGameObject:
      object = std::make_unique<GameObject>(game_object_id);
      break;
  }
  object->SetParams(params);
  game_objects_.emplace(std::make_pair(game_object_id, object));
  qInfo().noquote() << "[MODEL] Added new GameObject:" << game_object_id
    << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
                               .valueToKey(static_cast<uint32_t>(type)));
}

GameObjectId GameDataModel::AddGameObject(GameObjectType type,
                                          const std::vector<QVariant>& params) {
  GameObjectId game_object_id = this->GetNextUnusedGameObjectId();
  AddGameObject(game_object_id, type, params);
  return game_object_id;
}

void GameDataModel::DeleteGameObject(GameObjectId game_object_id) {
  auto iter = game_objects_.find(game_object_id);
  GameObjectType type = iter->second->GetType();
  if (iter != game_objects_.end()) {
    game_objects_.erase(iter);
  }
  qInfo().noquote() << "[MODEL] Removed GameObject:" << game_object_id
    << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
        .valueToKey(static_cast<uint32_t>(type)));
}

void GameDataModel::SetLocalPlayerId(GameObjectId player_id) {
  local_player_id_ = player_id;
  this->GetLocalPlayer()->SetIsLocalPlayer(true);
}

std::vector<std::shared_ptr<Player>> GameDataModel::GetPlayers() const {
  std::vector<std::shared_ptr<Player>> result;
  for (const auto& game_object : game_objects_) {
    if (game_object.second->GetType() == GameObjectType::kPlayer) {
      result.push_back(std::dynamic_pointer_cast<Player>(game_object.second));
    }
  }
  return result;
}

bool GameDataModel::IsGameObjectIdTaken(GameObjectId game_object_id) const {
  return game_objects_.find(game_object_id) != game_objects_.end();
}

GameObjectId GameDataModel::GetNextUnusedGameObjectId() const {
  GameObjectId game_object_id = 1;
  while (IsGameObjectIdTaken(game_object_id)) {
    game_object_id++;
  }
  return game_object_id;
}

std::vector<std::shared_ptr<MovableObject>>
  GameDataModel::GetMovableObjects() const {
  std::vector<std::shared_ptr<MovableObject>> result;
  for (const auto& object : game_objects_) {
    if (object.second->IsMovable()) {
      result.push_back(std::dynamic_pointer_cast<MovableObject>(object.second));
    }
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameDataModel::GetConstantObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    if (!object.second->IsMovable()) {
      result.push_back(object.second);
    }
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>> GameDataModel::GetAllGameObjects()
  const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    result.push_back(object.second);
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameDataModel::GetFilteredByFovObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    if (object.second->IsFilteredByFov()) {
      result.push_back(object.second);
    }
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameDataModel::GetNotFilteredByFovObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    if (!object.second->IsFilteredByFov()) {
      result.push_back(object.second);
    }
  }
  return result;
}
