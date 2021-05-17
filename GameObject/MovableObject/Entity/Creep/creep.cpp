#include "creep.h"

Creep::Creep(GameObjectId player_id) : Entity(player_id) {}

void Creep::DrawRelatively(Painter* painter) {
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

bool Creep::IsGoingToSpawn() const {
  return is_going_to_spawn_;
}

void Creep::SetIsGoingToSpawn(bool is_going_to_spawn) {
  is_going_to_spawn_ = is_going_to_spawn;
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
