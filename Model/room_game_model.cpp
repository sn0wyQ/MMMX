#include <utility>

#include "Hash/hash_calculator.h"
#include "room_game_model.h"

RoomGameModel::RoomGameModel(const RoomGameModel& model) : GameModel(model) {
  next_game_object_id_ = model.next_game_object_id_;
  last_object_hash_ = model.last_object_hash_;
  last_player_stats_hash_ = model.last_player_stats_hash_;
  next_bullet_id_ = model.next_bullet_id_;
}

GameObjectId RoomGameModel::GenerateNextUnusedGameObjectId() {
  return next_game_object_id_++;
}

GameObjectId RoomGameModel::AddGameObject(GameObjectType type,
                                          const std::vector<QVariant>& params) {
  GameObjectId game_object_id = this->GenerateNextUnusedGameObjectId();
  GameModel::AddGameObject(game_object_id, type, params);
  return game_object_id;
}

bool RoomGameModel::IsNeededToSendGameObjectData(
    GameObjectId game_object_id) const {
  auto object_iter = last_object_hash_.find(game_object_id);
  if (object_iter == last_object_hash_.end()) {
    return true;
  }
  auto hash = HashCalculator::GetHash(
      this->GetGameObjectByGameObjectId(game_object_id)->GetParams());
  return hash != object_iter->second;
}

void RoomGameModel::UpdateGameObjectHashes() {
  for (const auto& object : this->GetAllGameObjects()) {
    last_object_hash_[object->GetId()] =
        HashCalculator::GetHash(object->GetParams());
  }
}

void RoomGameModel::AddPlayerStats(GameObjectId player_id, QString nickname,
                               int level) {
  players_stats_[player_id] = std::make_shared<PlayerStats>(
      PlayerStats(player_id, std::move(nickname), level));
}

bool RoomGameModel::IsNeededToSendPlayerStats(GameObjectId player_id) {
  auto stats_iter = last_player_stats_hash_.find(player_id);
  if (stats_iter == last_player_stats_hash_.end()) {
    return true;
  }
  auto hash = HashCalculator::GetHash(
      this->GetPlayerStatsByPlayerId(player_id)->GetParams());
  return hash != stats_iter->second;
}

void RoomGameModel::UpdatePlayerStatsHashes() {
  for (const auto& stats : this->GetAllPlayersStats()) {
    last_player_stats_hash_[stats->GetPlayerId()] =
        HashCalculator::GetHash(stats->GetParams());
  }
}

QPointF RoomGameModel::GetPointToSpawn(float radius_from_object,
                                       bool for_player) const {
  std::uniform_real_distribution<> random_x(
      -Constants::kDefaultMapWidth / 2.f + radius_from_object,
      Constants::kDefaultMapWidth / 2.f - radius_from_object);
  std::uniform_real_distribution<> random_y(
      -Constants::kDefaultMapHeight / 2.f + radius_from_object,
      Constants::kDefaultMapHeight / 2.f - radius_from_object);
  int times_generate = 0;
  QPointF best_point;
  float big_distance{Constants::kDefaultMapHeight};
  float best_point_min_player_distance{0.f};
  while (true) {
    times_generate++;
    bool regenerate = false;
    static std::mt19937 rng(QDateTime::currentMSecsSinceEpoch());
    float min_player_distance{big_distance};
    QPointF point(random_x(rng), random_y(rng));
    for (const auto& game_object : GetAllGameObjects()) {
      if (game_object->GetType() == GameObjectType::kMapBorder) {
        continue;
      }
      if (for_player) {
        if (game_object->GetType() == GameObjectType::kPlayer) {
          min_player_distance =
              std::min(min_player_distance, Math::DistanceBetweenPoints(
                  point, game_object->GetPosition()));
        }
      }
      float bounding_circle_radius =
          game_object->GetRigidBodyBoundingCircleRadius();
      if (Math::DistanceBetweenPoints(point, game_object->GetPosition()) <
          radius_from_object + bounding_circle_radius) {
        regenerate = true;
      }
    }
    if (!regenerate) {
      if (for_player) {
        if (min_player_distance >= best_point_min_player_distance) {
          best_point_min_player_distance = min_player_distance;
          best_point = point;
        }
      } else {
        return point;
      }
    }
    if (times_generate == 1000) {
      if (for_player) {
        break;
      }
      // костыль если вся карта занята
      // я тут хз что делать, крашить сервер не хочется,
      // так что пока оставил создание объекта где то далеко
      qWarning() << "[MODEL] Can't find spawn point";
      return QPointF(Constants::kDefaultMapWidth,
                     Constants::kDefaultMapHeight);
    }
  }
  return best_point;
}

GameObjectId RoomGameModel::GenerateNextUnusedBulletId(GameObjectId player_id) {
  if (next_bullet_id_.find(player_id) == next_bullet_id_.end()) {
    next_bullet_id_[player_id] = 1;
  } else {
    next_bullet_id_[player_id]++;
  }
  return player_id * Constants::kMaxPlayerBullets + next_bullet_id_[player_id];
}
