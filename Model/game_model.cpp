#include "game_model.h"

GameModel::GameModel(const GameModel& other) {
  for (const auto& [game_object_id, game_object] : other.game_objects_) {
    game_objects_.emplace(game_object_id, game_object->Clone());
  }
}

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

std::shared_ptr<GameObject> GameModel::GetNewEmptyGameObject(
    GameObjectId game_object_id, GameObjectType type) {
  std::shared_ptr<GameObject> object;
  switch (type) {
    case GameObjectType::kPlayer:
      object = std::make_unique<Player>(game_object_id);
      break;
    case GameObjectType::kGameObject:
      object = std::make_unique<GameObject>(game_object_id);
      break;
    case GameObjectType::kBullet:
      object = std::make_unique<Bullet>(game_object_id);
      break;
  }
  return object;
}

void GameModel::AddGameObject(GameObjectId game_object_id,
                                  GameObjectType type,
                                  const std::vector<QVariant>& params) {
  if (game_objects_.find(game_object_id) != game_objects_.end()) {
    throw std::runtime_error("[MODEL] This game_object_id already exists");
  }
  std::shared_ptr<GameObject> object =
      GetNewEmptyGameObject(game_object_id, type);
  object->SetParams(params);
  game_objects_.emplace(std::make_pair(game_object_id, object));
  qInfo().noquote() << "[MODEL] Added new GameObject:" << game_object_id
                    << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
                        .valueToKey(static_cast<uint32_t>(type)));
}

void GameModel::DeleteGameObject(GameObjectId game_object_id) {
  auto iter = game_objects_.find(game_object_id);
  if (iter == game_objects_.end()) {
    qInfo() << "gay";
    return;
  }
  GameObjectType type = iter->second->GetType();
  game_objects_.erase(iter);
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

std::vector<std::shared_ptr<Bullet>> GameModel::GetAllBullets() const {
  std::vector<std::shared_ptr<Bullet>> result;
  for (const auto& object : game_objects_) {
    if (object.second->GetType() == GameObjectType::kBullet) {
      result.push_back(std::dynamic_pointer_cast<Bullet>(object.second));
    }
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

void GameModel::AttachGameObject(
    GameObjectId game_object_id,
    const std::shared_ptr<GameObject>& game_object) {
  game_objects_[game_object_id] = game_object;
  qInfo().noquote() << "[MODEL] Added new GameObject:" << game_object_id
     << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
         .valueToKey(static_cast<uint32_t>(game_object->GetType())));
}
