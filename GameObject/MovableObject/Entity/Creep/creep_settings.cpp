#include <algorithm>
#include <random>
#include <utility>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLineF>

#include "creep_settings.h"
#include "GameObject/RigidBody/rigid_body.h"

CreepSettings* CreepSettings::instance_{nullptr};

CreepSettings& CreepSettings::GetInstance() {
  if (!instance_) {
    instance_ = new CreepSettings();
  }
  return *instance_;
}

CreepSettings::CreepSettings() {
  QFile file(":creep_settings.json");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "File cannot be open";
  }
  json_object_ = QJsonDocument::fromJson(file.readAll()).object();
}

QSizeF CreepSettings::GetCreepSizeByLevel(int level) const {
  return QSizeF(this->CalculateWidth(level),
                this->CalculateHeight(level));
}

int CreepSettings::GetCreepLevel(float distance_from_center) const {
  static std::mt19937 gen(QDateTime::currentMSecsSinceEpoch());
  auto min_max_level =
      this->GetMinAndMaxCreepLevelByDistance(distance_from_center);
  return std::uniform_int_distribution<int>(min_max_level.first,
                                            min_max_level.second)(gen);
}

std::vector<QVariant>
     CreepSettings::GetCreepParams(float x, float y,
                                   float distance_from_center) const {
  std::vector<QVariant> params;
  int creep_level = this->GetCreepLevel(distance_from_center);

  CreepType creep_type = this->CalculateType(creep_level);
  float
      rotation = static_cast<float>(QRandomGenerator::global()->bounded(360.f));
  float width = this->CalculateWidth(creep_level);
  float height = 0.f;
  RigidBodyType rigid_body_type = RigidBodyType::kCircle;
  AnimationType animation_type = AnimationType::kNone;
  float fov = 0.1f;
  switch (creep_type) {
    case CreepType::kBoxSciFiCube:
      height = width;
      rigid_body_type = RigidBodyType::kRectangle;
      animation_type = AnimationType::kBoxSciFiCube;
      fov = 0.1f;
      break;

    case CreepType::kTemporaryCircle:
      height = width;
      rigid_body_type = RigidBodyType::kCircle;
      animation_type = AnimationType::kNone;
      fov = this->CalculateFov(creep_level);
      break;

    case CreepType::kBoxSciFiLong:
      height = width * (384.f / 228.f);
      rigid_body_type = RigidBodyType::kRectangle;
      animation_type = AnimationType::kBoxSciFiLong;
      fov = 0.1f;
      break;

    default:
      qWarning() << "[CREEP] Unknown creep type";
      break;
  }

  // GameObject params
  params.emplace_back(x);
  params.emplace_back(y);
  params.emplace_back(rotation);
  params.emplace_back(width);
  params.emplace_back(height);
  params.emplace_back(static_cast<int>(rigid_body_type));
  params.emplace_back(width);
  params.emplace_back(height);
  params.emplace_back(static_cast<int>(animation_type));
  // MovableObject params
  params.emplace_back(0);  // velocity_x
  params.emplace_back(0);  // velocity_y
  params.emplace_back(this->CalculateSpeed(creep_level));
  // Entity params
  params.emplace_back(fov);
  params.emplace_back(this->CalculateMaxHp(creep_level));
  params.emplace_back(this->CalculateRegenRate(creep_level));
  params.emplace_back(this->CalculateMaxHp(creep_level));
  // Creep params
  params.emplace_back(creep_level);
  params.emplace_back(this->CalculateExpIncrement(creep_level));
  return params;
}

float CreepSettings::CalculateExpIncrement(int level) const {
  return level;
}

float CreepSettings::CalculateMaxHp(int level) const {
  return this->GetCreepSetting<float>("max_hp_multiplier") * level;
}

float CreepSettings::CalculateRegenRate(int level) const {
  return level * this->GetCreepSetting<float>("hp_regen_rate_multiplier");
}

float CreepSettings::CalculateWidth(int level) const {
  return std::min(this->GetCreepSetting<float>("max_width"),
      0.5f * level);
}

float CreepSettings::CalculateHeight(int level) const {
  return std::min(this->GetCreepSetting<float>("max_height"),
      0.5f * level);
}

std::pair<int, int>
     CreepSettings::GetMinAndMaxCreepLevelByDistance(float distance) const {
  auto map_radius = QLineF(QPointF(0, 0),
                           QPointF(Constants::kDefaultMapHeight,
                                       Constants::kDefaultMapWidth)).length();
  auto level_multiplier = (1 - distance / map_radius);
  float min_level = this->GetCreepSetting<int>("min_creep_level") *
       std::pow(level_multiplier,
                this->GetCreepSetting<float>("kMinLevelPow"));
  float max_level =  this->GetCreepSetting<int>("max_creep_level") *
      std::pow(level_multiplier,
               this->GetCreepSetting<float>("kMaxLevelPow"));;
  min_level = std::max(1.f, min_level);
  max_level = std::max(1.f, max_level);
  return {min_level, max_level};
}

QSizeF CreepSettings::GetMaxCreepSize() const {
  return QSizeF(this->GetCreepSetting<float>("max_width"),
                this->GetCreepSetting<float>("max_height"));
}

float CreepSettings::CalculateSpeed(int level) const {
  float max_level = this->GetCreepSetting<int>("max_creep_level");
  return (max_level - level) * this->GetCreepSetting<float>("speed_multiplier");
}

CreepType CreepSettings::CalculateType(int level) const {
  int random = QRandomGenerator::global()->bounded(1, 10);
  if (random <= 3) {  // 1 - 3
    return CreepType::kBoxSciFiCube;
  } else if (random <= 6) {  // 4 - 6
    return CreepType::kBoxSciFiLong;
  } else {  // 7 - 10
    return CreepType::kTemporaryCircle;
  }
}

float CreepSettings::CalculateFov(int level) const {
  int random = QRandomGenerator::global()->bounded(1, 10);
  if (random <= 3) {
    return 0.1f;
  } else {
    return 20.f;
  }
}

float CreepSettings::CalculateDamage(int level) const {
  return this->GetCreepSetting<float>("damage_multiplier") * level;
}

void CreepSettings::SetStaticParams(const std::shared_ptr<Creep>& creep) {
  creep->SetSpawnX(creep->GetX());
  creep->SetSpawnY(creep->GetY());
  int creep_level = creep->GetLevel();
  creep->SetFovRadius(
      CreepSettings::GetInstance().CalculateFov(creep_level));
  creep->SetAttackDistance(
      CreepSettings::GetInstance().GetCreepSetting<float>("attack_distance"));
  creep->SetDamage(CreepSettings::GetInstance().CalculateDamage(creep_level));
  creep->SetReloadingTime(
      CreepSettings::GetInstance().GetCreepSetting<float>("reloading_time"));
}
