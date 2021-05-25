#include "creep.h"
#include "creep_settings.h"

Creep::Creep(GameObjectId player_id) : Entity(player_id) {}

void Creep::DrawRelatively(Painter* painter) const {
  painter->DrawEllipse(QPointF(),
                       this->GetWidth() / 2.f - 0.1f,
                       this->GetHeight() / 2.f - 0.1f);
}

GameObjectType Creep::GetType() const {
  return GameObjectType::kCreep;
}

void Creep::SetParams(std::vector<QVariant> params) {
  this->SetExpIncrementForKill(params.back().toFloat());
  params.pop_back();
  this->SetCreepType(static_cast<CreepType>(params.back().toInt()));
  params.pop_back();
  this->SetLevel(params.back().toInt());
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Creep::GetParams() const {
  auto params = Entity::GetParams();
  params.emplace_back(this->GetLevel());
  params.emplace_back(static_cast<int>(this->GetCreepType()));
  params.emplace_back(this->GetExpIncrementForKill());
  return params;
}

void Creep::SetExpIncrementForKill(float exp_increment) {
  exp_increment_ = exp_increment;
}

float Creep::GetExpIncrementForKill() const {
  return exp_increment_;
}

float& Creep::GetMutableExpIncrementForKill() {
  return exp_increment_;
}

std::shared_ptr<GameObject> Creep::Clone() const {
  return std::make_shared<Creep>(*this);
}

float Creep::GetSpawnX() const {
  return spawn_x_;
}

float Creep::GetSpawnY() const {
  return spawn_y_;
}

void Creep::SetSpawnX(float spawn_x) {
  Creep::spawn_x_ = spawn_x;
}

void Creep::SetSpawnY(float spawn_y) {
  Creep::spawn_y_ = spawn_y;
}

void Creep::SetAttackDistance(float attack_distance) {
  attack_distance_ = attack_distance;
}

float Creep::GetAttackDistance() const {
  return attack_distance_;
}

float Creep::GetDamage() const {
  return damage_;
}

void Creep::SetDamage(float damage) {
  damage_ = damage;
}

int64_t Creep::GetLastAttackedTime() const {
  return last_attacked_time_;
}

int64_t Creep::GetReloadingTime() const {
  return reloading_time_;
}

void Creep::SetLastAttackedTime(int64_t last_attacked_time) {
  last_attacked_time_ = last_attacked_time;
}

void Creep::SetReloadingTime(int64_t reloading_time) {
  reloading_time_ = reloading_time;
}

bool Creep::IsPossibleToAttack(int64_t timestamp) const {
  if (GetDamage() < Math::kEps) {
    return false;
  }
  return (timestamp - last_attacked_time_) >= reloading_time_;
}

void Creep::SetCreepType(CreepType creep_type) {
  creep_type_ = creep_type;
}

CreepType Creep::GetCreepType() const {
  return creep_type_;
}

void Creep::TickIntelligence(
    const std::vector<std::shared_ptr<GameObject>>& game_objects) {
  if (!CreepSettings::GetInstance().HasIntelligence(this->GetCreepType())) {
    return;
  }

  std::shared_ptr<Player> focused_player;
  std::vector<std::shared_ptr<GameObject>> nearby_game_objects;
  std::shared_ptr<GameObject> map_border;
  bool is_revenging = false;
  std::shared_ptr<Player> last_aggressive_player;

  for (const auto& game_object : game_objects) {
    if (game_object->GetType() == GameObjectType::kPlayer &&
        game_object->GetId() == last_aggressive_player_) {
      last_aggressive_player = std::dynamic_pointer_cast<Player>(game_object);
    }
    if (game_object->GetType() == GameObjectType::kMapBorder) {
      map_border = game_object;
    } else if ((!game_object->IsEntity() ||
        game_object->GetType() == GameObjectType::kCreep ||
        (game_object->GetType() == GameObjectType::kPlayer &&
            !game_object->IsAlive())) &&
        Math::DistanceBetweenPoints(this->GetSpawnPoint(),
                                    game_object->GetPosition())
            < this->GetFovRadius() +
                game_object->GetRigidBodyBoundingCircleRadius() &&
        game_object->GetId() != this->GetId()) {
      nearby_game_objects.push_back(game_object);
    }
  }

  if (last_aggressive_player &&
      QDateTime::currentMSecsSinceEpoch() - last_taken_damage_time_ <
          Constants::kRevengingTimeMSecs) {
    is_revenging = true;
    focused_player = last_aggressive_player;
  } else {
    float best_distance = 1e9;
    for (const auto& player : game_objects) {
      if (player->GetType() == GameObjectType::kPlayer) {
        bool visible = true;
        for (const auto& game_object : nearby_game_objects) {
          if (!game_object->IsEntity() &&
              game_object->Intersects(Math::Line(this->GetPosition(),
                                                 player->GetPosition()))) {
            visible = false;
            break;
          }
        }
        if (!visible) {
          continue;
        }
        auto distance = Math::DistanceBetweenPoints(this->GetPosition(),
                                                    player->GetPosition());
        if (distance < best_distance && player->IsAlive()) {
          focused_player = std::dynamic_pointer_cast<Player>(player);
          best_distance = distance;
        }
      }
    }
  }

  bool is_patrolling = true;
  QPointF point_to_go;
  if (is_revenging ||
      (focused_player &&
          Math::DistanceBetweenPoints(focused_player->GetPosition(),
                                      this->GetSpawnPoint()) <
              this->GetFovRadius())) {
    point_to_go = focused_player->GetPosition();
    is_patrolling = false;
  } else {
    point_to_go = this->GetPatrollingPoint(nearby_game_objects, map_border);
  }

  force_ = QVector2D(point_to_go - this->GetPosition());

  auto runaway_hp = this->GetMaxHealthPoints()
      * CreepSettings::GetInstance().GetRunawayHpRatio(this->GetCreepType());
  if (!is_patrolling && this->GetHealthPoints() < runaway_hp) {
    force_ *= -Constants::kRunawaySpeedMultiplier;
    this->SetRotation(Math::GetNormalizeAngle(
        180 + Math::VectorAngle(this->GetPosition(), point_to_go)));
  } else {
    this->SetRotation(Math::VectorAngle(this->GetPosition(), point_to_go));
  }
  force_.normalize();
  if (is_patrolling) {
    force_ /= Constants::kPatrollingSpeedDivider;
  }
}

QVector2D Creep::GetForce() const {
  return force_;
}

GameObjectId Creep::GetPlayerToDamage(
    const std::vector<std::shared_ptr<Player>>& players) const {
  if (!CreepSettings::GetInstance().HasIntelligence(this->GetCreepType())) {
    return Constants::kNullGameObjectId;
  }

  std::shared_ptr<Player> closer_player{nullptr};
  float best_distance = std::numeric_limits<float>::max();
  for (const auto& player : players) {
    if (!player->IsAlive()) {
      continue;
    }
    auto distance = Math::DistanceBetweenPoints(this->GetPosition(),
                                                player->GetPosition());
    if (distance < best_distance) {
      closer_player = player;
      best_distance = distance;
    }
  }
  if (closer_player) {
    if (best_distance <
        closer_player->GetRigidBodyBoundingCircleRadius()
            + this->GetRigidBodyBoundingCircleRadius()
            + this->GetAttackDistance()) {
      return closer_player->GetId();
    }
  }
  return Constants::kNullGameObjectId;
}

void Creep::SetAggressivePlayer(const std::shared_ptr<Player>& player) {
  last_aggressive_player_ = player->GetId();
  last_taken_damage_time_ = QDateTime::currentMSecsSinceEpoch();
}

QPointF Creep::GetSpawnPoint() const {
  return QPointF(spawn_x_, spawn_y_);
}

QPointF Creep::GetPatrollingPoint(
    const std::vector<std::shared_ptr<GameObject>>& nearby_game_objects,
    const std::shared_ptr<GameObject>& map_border) {
  static std::mt19937 gen(QDateTime::currentSecsSinceEpoch());
  static std::uniform_real_distribution<float> get_phi(0.f, 2.f * Math::kPi);
  static std::uniform_real_distribution<float> get_rho(
      this->GetRigidBodyBoundingCircleRadius(), this->GetFovRadius());

  for (int i = 0; i < Constants::kTriesToFindPatrollingPoint; i++) {
    auto dist_from_spawn =
        Math::DistanceBetweenPoints(this->GetSpawnPoint(), patrolling_point_);
    auto dist_from_current_pos =
        Math::DistanceBetweenPoints(this->GetPosition(), patrolling_point_);
    if (!patrolling_point_.isNull() &&
        dist_from_current_pos > this->GetRigidBodyBoundingCircleRadius() &&
        dist_from_spawn < this->GetFovRadius() &&
        !map_border->Intersects(Math::Line(this->GetPosition(),
                                           patrolling_point_))) {
      bool current_point_is_good = true;
      for (const auto& object : nearby_game_objects) {
        auto dist_from_object =
            Math::DistanceBetweenPoints(patrolling_point_,
                                        object->GetPosition());
        auto min_dist_between_objects =
            object->GetRigidBodyBoundingCircleRadius() +
            this->GetRigidBodyBoundingCircleRadius();
        if (dist_from_object < min_dist_between_objects ||
            object->Intersects(Math::Line(this->GetPosition(),
                                          patrolling_point_))) {
          current_point_is_good = false;
          break;
        }
      }
      if (current_point_is_good) {
        break;
      }
    }
    auto phi = get_phi(gen);
    auto rho = get_rho(gen);
    patrolling_point_ = this->GetSpawnPoint() + QPointF(rho * std::cos(phi),
                                                        rho * std::sin(phi));
  }
  return patrolling_point_;
}
