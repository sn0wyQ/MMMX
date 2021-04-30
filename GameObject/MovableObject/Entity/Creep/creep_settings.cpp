#include <algorithm>
#include <random>
#include <utility>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLineF>

#include "creep_settings.h"

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
     CreepSettings::GetCreepParams(float x, float y, float rotation,
                                   float distance_from_center) const {
  std::vector<QVariant> params;
  int creep_level = this->GetCreepLevel(distance_from_center);
  // GameObject params
  params.emplace_back(x);
  params.emplace_back(y);
  params.emplace_back(rotation);
  params.emplace_back(this->CalculateWidth(creep_level));
  params.emplace_back(this->CalculateHeight(creep_level));
  params.emplace_back(json_object_.value("rigid_body_type").toInt());
  // MovableObject params
  params.emplace_back(0);  // velocity_x
  params.emplace_back(0);  // velocity_y
  // Entity params
  params.emplace_back(0.f);  // FOV for creep is always 0
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