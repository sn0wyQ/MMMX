#include "game_data_model.h"

std::shared_ptr<Player>
    GameDataModel::GetPlayerByPlayerId(GameObjectId player_id) const {
  auto iter = game_objects_.find(player_id);
  if (iter != game_objects_.end()
    && iter->second->GetGameObjectType() == GameObjectType::kPlayer) {
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

int GameDataModel::GetPlayersCount() const {
  return GetPlayers().size();
}

void GameDataModel::AddGameObject(GameObjectId game_object_id,
                                  GameObjectType type,
                                  const std::vector<QVariant>& params) {
  if (game_objects_.find(game_object_id) != game_objects_.end()) {
    throw std::runtime_error("[MODEL] This game_object_id already exists");
  }
  switch (type) {
    case GameObjectType::kPlayer:
      game_objects_.emplace(std::make_pair(
          game_object_id,
          std::make_unique<Player>(game_object_id, params)));
      break;
    case GameObjectType::kBox:
      game_objects_.emplace(std::make_pair(
          game_object_id,
          std::make_unique<Box>(game_object_id, params)));
      break;
    case GameObjectType::kTree:
      game_objects_.emplace(std::make_pair(
          game_object_id,
          std::make_unique<Tree>(game_object_id, params)));
      break;
  }
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
  GameObjectType type = iter->second->GetGameObjectType();
  if (iter != game_objects_.end()) {
    game_objects_.erase(iter);
  }
  qInfo().noquote() << "[MODEL] Removed GameObject:" << game_object_id
    << "type =" << QString(QMetaEnum::fromType<GameObjectType>()
        .valueToKey(static_cast<uint32_t>(type)));
}

void GameDataModel::SetLocalPlayerId(GameObjectId player_id) {
  local_player_id_ = player_id;
  GetLocalPlayer()->SetIsLocalPlayer(true);
}

std::vector<std::shared_ptr<Player>> GameDataModel::GetPlayers() const {
  std::vector<std::shared_ptr<Player>> result;
  for (const auto& game_object : game_objects_) {
    if (game_object.second->GetGameObjectType() == GameObjectType::kPlayer) {
      result.push_back(std::dynamic_pointer_cast<Player>(game_object.second));
    }
  }
  return result;
}

std::vector<std::shared_ptr<Box>> GameDataModel::GetBoxes() const {
  std::vector<std::shared_ptr<Box>> result;
  for (const auto& game_object : game_objects_) {
    if (game_object.second->GetGameObjectType() == GameObjectType::kBox) {
      result.push_back(std::dynamic_pointer_cast<Box>(game_object.second));
    }
  }
  return result;
}

std::vector<std::shared_ptr<Tree>> GameDataModel::GetTrees() const {
  std::vector<std::shared_ptr<Tree>> result;
  for (const auto& game_object : game_objects_) {
    if (game_object.second->GetGameObjectType() == GameObjectType::kTree) {
      result.push_back(std::dynamic_pointer_cast<Tree>(game_object.second));
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
  std::vector<std::shared_ptr<Player>> players = GetPlayers();
  std::vector<std::shared_ptr<MovableObject>> result;
  result.reserve(players.size());
  for (const auto& player : players) {
    result.push_back(player);
  }
  return result;
}

std::vector<std::shared_ptr<RoundStaticObject>>
  GameDataModel::GetRoundStaticObjects() const {
  std::vector<std::shared_ptr<Tree>> trees = GetTrees();
  std::vector<std::shared_ptr<RoundStaticObject>> result;
  result.reserve(trees.size());
  for (const auto& tree : trees) {
    result.push_back(tree);
  }
  return result;
}

std::vector<std::shared_ptr<RectangularStaticObject>>
  GameDataModel::GetRectangularStaticObjects() const {
  std::vector<std::shared_ptr<Box>> boxes = GetBoxes();
  std::vector<std::shared_ptr<RectangularStaticObject>> result;
  result.reserve(boxes.size());
  for (const auto& box : boxes) {
    result.push_back(box);
  }
  return result;
}

std::vector<std::shared_ptr<GameObject>>
  GameDataModel::GetConstantObjects() const {
  std::vector<std::shared_ptr<RectangularStaticObject>> rectangular_static
    = GetRectangularStaticObjects();
  std::vector<std::shared_ptr<RoundStaticObject>> round_static
    = GetRoundStaticObjects();
  std::vector<std::shared_ptr<GameObject>> result;
  result.reserve(rectangular_static.size() + round_static.size());
  for (const auto& object : rectangular_static) {
    result.push_back(object);
  }
  for (const auto& object : round_static) {
    result.push_back(object);
  }
  return result;
}
