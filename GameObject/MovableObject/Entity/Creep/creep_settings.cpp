#include <utility>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

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
  QJsonArray
      json_weapon_array(QJsonDocument::fromJson(file.readAll()).array());
  for (int i = 0; i < json_weapon_array.size(); i++) {
    auto object = json_weapon_array[i].toObject();
    json_object_by_level_[object.value("level").toInt()] = object;
  }
}

std::vector<QVariant> CreepSettings::GetCreepParams(int creep_level,
                                                    float x,
                                                    float y,
                                                    float rotation) const {
  std::vector<QVariant> params;
  auto object = json_object_by_level_.at(creep_level);

  // GameObject params
  params.emplace_back(x);
  params.emplace_back(y);
  params.emplace_back(rotation);
  params.emplace_back(
      object.value("width").toVariant().toFloat());
  params.emplace_back(
      object.value("height").toVariant().toFloat());
  params.emplace_back(
      object.value("rigid_body_type").toInt());
  // MovableObject params
  params.emplace_back(0);  // velocity_x
  params.emplace_back(0);  // velocity_y
  // Entity params
  params.emplace_back(0.f);  // FOV for creep is always 0
  params.emplace_back(
      object.value("default_hp").toVariant().toFloat());
  params.emplace_back(
      object.value("hp_regen_rate").toVariant().toFloat());
  params.emplace_back(
      object.value("max_hp").toVariant().toFloat());
  // Creep params
  params.emplace_back(
      object.value("level").toInt());
  params.emplace_back(
      object.value("exp_increment_for_kill").toVariant().toFloat());
  return params;
}

