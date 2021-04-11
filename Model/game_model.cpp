#include "game_model.h"

std::shared_ptr<Player> GameModel::GetPlayerByPlayerId(
    GameObjectId player_id) const {
  auto iter = game_objects_.find(player_id);
  if (iter != game_objects_.end() &&
      iter->second->GetType() == GameObjectType::kPlayer) {
    return std::dynamic_pointer_cast<Player>(iter->second);
  }
  throw std::runtime_error("[MODEL] Trying to get invalid player...");
}

std::shared_ptr<GameObject> GameModel::GetGameObjectByGameObjectId(
    GameObjectId game_object_id) const {
  auto iter = game_objects_.find(game_object_id);
  if (iter != game_objects_.end()) {
    return iter->second;
  }
  throw std::runtime_error("[MODEL] Trying to get invalid game object...");
}

void GameModel::AddGameObject(GameObjectId game_object_id,
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

void GameModel::DeleteGameObject(GameObjectId game_object_id) {
  auto iter = game_objects_.find(game_object_id);
  GameObjectType type = iter->second->GetType();
  if (iter != game_objects_.end()) {
    game_objects_.erase(iter);
  }
  qInfo().noquote() << "[MODEL] Removed GameObject:" << game_object_id
    << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
        .valueToKey(static_cast<uint32_t>(type)));
}

std::vector<std::shared_ptr<Player>> GameModel::GetPlayers() const {
  std::vector<std::shared_ptr<Player>> result;
  for (const auto& game_object : game_objects_) {
    if (game_object.second->GetType() == GameObjectType::kPlayer) {
      result.push_back(std::dynamic_pointer_cast<Player>(game_object.second));
    }
  }
  return result;
}

bool GameModel::IsGameObjectIdTaken(GameObjectId game_object_id) const {
  return game_objects_.find(game_object_id) != game_objects_.end();
}

std::vector<std::shared_ptr<GameObject>>
  GameModel::GetAllGameObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    result.push_back(object.second);
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameModel::GetFilteredByFovObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    if (object.second->IsFilteredByFov()) {
      result.push_back(object.second);
    }
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameModel::GetNotFilteredByFovObjects() const {
  std::vector<std::shared_ptr<GameObject>> result;
  for (const auto& object : game_objects_) {
    if (!object.second->IsFilteredByFov()) {
      result.push_back(object.second);
    }
  }
  return result;
}