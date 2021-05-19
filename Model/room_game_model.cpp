#include <random>
#include <utility>

#include "Hash/hash_calculator.h"
#include "room_game_model.h"

using Constants::kChunksX;
using Constants::kChunksY;

RoomGameModel::RoomGameModel(const RoomGameModel& model) : GameModel(model) {
  next_game_object_id_ = model.next_game_object_id_;
  last_object_hash_ = model.last_object_hash_;
  last_player_stats_hash_ = model.last_player_stats_hash_;
  next_bullet_id_for_player_ = model.next_bullet_id_for_player_;
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
                                       GameObjectType game_object_type) const {
  bool for_player = (game_object_type == GameObjectType::kPlayer);
  bool for_creep = (game_object_type == GameObjectType::kCreep);
  static std::mt19937 gen(QDateTime::currentSecsSinceEpoch());
  QPointF chunk_size(
      (Constants::kMapWidth - 2 * radius_from_object) / kChunksX,
      (Constants::kMapHeight - 2 * radius_from_object) / kChunksY);
  QPointF top_left_dot(
      -Constants::kMapWidth / 2.f + radius_from_object,
      -Constants::kMapHeight / 2.f + radius_from_object);
  std::vector<QRectF> chunks;
  for (int i = 0; i < kChunksX; i++) {
    for (int j = 0; j < kChunksY; j++) {
      QRectF current_chunk(top_left_dot.x() + chunk_size.x() * i,
                           top_left_dot.y() + chunk_size.y() * j,
                           chunk_size.x(), chunk_size.y());
      chunks.emplace_back(current_chunk);
    }
  }

  if (for_creep) {
    std::uniform_real_distribution<double> real_distribution(0.01f, 20.f);
    std::vector<std::pair<QRectF, double>> distances(chunks.size());
    double max_dist =
        Math::DistanceBetweenPoints(QPointF(),
                                    QPointF(Constants::kMapWidth,
                                            Constants::kMapHeight));
    for (size_t i = 0; i < distances.size(); i++) {
      auto distance =
          Math::DistanceBetweenPoints(QPointF(), chunks[i].center()) / max_dist;
      distance *= real_distribution(gen);
      distances[i] = {chunks[i], distance};
    }
    std::sort(distances.begin(), distances.end(),
              [](const std::pair<QRectF, double>& pair1,
                 const std::pair<QRectF, double>& pair2) {
      return pair1.second < pair2.second;
    });
    chunks.clear();
    for (auto [chunk, _] : distances) {
      chunks.push_back(chunk);
    }
  } else {
    std::vector<std::pair<QRectF, int>> objects_in_chunk(chunks.size());
    for (auto chunk : chunks) {
      int count_of_objects = 0;
      for (const auto& game_object : this->GetAllGameObjects()) {
        if (chunk.contains(game_object->GetPosition())) {
          ++count_of_objects;
        }
      }
      objects_in_chunk.emplace_back(chunk, count_of_objects);
    }
    std::sort(objects_in_chunk.begin(), objects_in_chunk.end(),
              [](const std::pair<QRectF, int>& pair1,
                 const std::pair<QRectF, int>& pair2) {
      if (pair1.second == pair2.second) {
        return pair1.first.x() < pair2.first.x();
      }
      return pair1.second < pair2.second;
    });
    int last_index = 0;
    for (int i = 0; i < static_cast<int>(objects_in_chunk.size()); i++) {
      if (objects_in_chunk[last_index].second !=
          objects_in_chunk[i].second) {
        std::shuffle(objects_in_chunk.begin() + last_index,
                     objects_in_chunk.begin() + i, gen);
        last_index = i;
      }
    }
    chunks.clear();
    for (auto [chunk, _] : objects_in_chunk) {
      chunks.push_back(chunk);
    }
  }

  std::vector<std::pair<float, QPointF>> points_to_spawn;
  for (auto chunk_rect : chunks) {
    std::uniform_real_distribution<> distribution_x(
        chunk_rect.topLeft().x(), chunk_rect.topRight().x());
    std::uniform_real_distribution<> distribution_y(
        chunk_rect.topLeft().y(), chunk_rect.bottomLeft().y());
    for (int tries = 0; tries < Constants::kGenerationTries; tries++) {
      QPointF point(distribution_x(gen), distribution_y(gen));
      bool can_spawn = true;
      float min_dist_from_player = 1e9;
      for (const auto& game_object : this->GetAllGameObjects()) {
        auto distance = Math::DistanceBetweenPoints(point,
                                                    game_object->GetPosition());
        if (for_player && game_object->IsEntity()) {
          min_dist_from_player = std::min(min_dist_from_player, distance);
        }
        auto good_dist = radius_from_object +
            game_object->GetRigidBodyBoundingCircleRadius();
        if (game_object->GetType() != GameObjectType::kMapBorder &&
            distance < good_dist) {
          can_spawn = false;
        }
      }
      if (can_spawn) {
        if (for_player) {
          points_to_spawn.emplace_back(min_dist_from_player, point);
        } else {
          return point;
        }
      }
    }
  }
  if (points_to_spawn.empty()) {
    qWarning() << "Cant find spawn point for object";
    return QPointF(1, 1);
  }
  std::sort(points_to_spawn.begin(), points_to_spawn.end(),
            [](const std::pair<float, QPointF>& pair1,
               const std::pair<float, QPointF>& pair2) {
              return pair1.first < pair2.first;
            });
  return points_to_spawn.back().second;
}

GameObjectId RoomGameModel::GenerateNextUnusedBulletId(GameObjectId player_id) {
  if (next_bullet_id_for_player_.find(player_id) ==
    next_bullet_id_for_player_.end()) {
    next_bullet_id_for_player_[player_id] = 1;
  } else {
    next_bullet_id_for_player_[player_id]++;
  }
  return player_id * Constants::kMaxPlayerBullets +
    next_bullet_id_for_player_[player_id];
}
