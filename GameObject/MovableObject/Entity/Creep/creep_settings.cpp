#include <utility>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <random>

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
  params.emplace_back(this->CalculateDefaultHp(creep_level));
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

float CreepSettings::CalculateDefaultHp(int level) const {
  return this->CalculateMaxHp(level)
      * this->GetCreepSetting<float>("default_hp_ratio");
}

float CreepSettings::CalculateRegenRate(int level) const {
  return level * this->GetCreepSetting<float>("hp_regen_rate_multiplier");
}

float CreepSettings::CalculateWidth(int level) const {
  return std::min(this->GetCreepSetting<float>("max_width"), 0.5f * level);
}

float CreepSettings::CalculateHeight(int level) const {
  return std::min(this->GetCreepSetting<float>("max_height"), 0.5f * level);
}

std::pair<int, int>
     CreepSettings::GetMinAndMaxCreepLevelByDistance(float distance) const {
  static std::mt19937 gen(QDateTime::currentMSecsSinceEpoch());
  static std::uniform_int_distribution<int>
      uid1(this->GetCreepSetting<int>("min_creep_level"),
           this->GetCreepSetting<int>("max_creep_level") / 2);
  static std::uniform_int_distribution<int>
      uid2(this->GetCreepSetting<int>("max_creep_level") / 2,
           this->GetCreepSetting<int>("max_creep_level"));
  int res{0};
  if (distance < 20.f) {
    res = uid2(gen);
  } else {
    res = uid1(gen);
  }
  return {res, res};
}

QSizeF CreepSettings::GetMaxCreepSize() const {
  return QSizeF(this->GetCreepSetting<float>("max_width"),
                this->GetCreepSetting<float>("max_height"));
}
