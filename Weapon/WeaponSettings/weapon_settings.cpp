#include <utility>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

#include "weapon_settings.h"

WeaponSettings* WeaponSettings::instance_{nullptr};

WeaponSettings& WeaponSettings::GetInstance() {
  if (!instance_) {
    instance_ = new WeaponSettings();
  }
  return *instance_;
}

WeaponSettings::WeaponSettings() {
  QFile file(":weapon_settings.json");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "File cannot be open";
  }

  QJsonArray
      json_weapon_array(QJsonDocument::fromJson(file.readAll()).array());
  std::vector<QString> weapon_types_;
  weapon_types_.reserve(static_cast<int>(WeaponType::SIZE));
  for (int i = 0; i < static_cast<int>(WeaponType::SIZE); i++) {
    weapon_types_.emplace_back(QMetaEnum::fromType<WeaponType>().valueToKey(0));
  }
  for (int i = 0; i < json_weapon_array.size(); i++) {
    auto object = json_weapon_array[i].toObject();
    std::vector<QVariant> params;
    params.emplace_back(object.value("accuracy"));
    params.emplace_back(object.value("bullet_damage"));
    params.emplace_back(object.value("bullet_range"));
    params.emplace_back(object.value("bullet_speed"));
    params.emplace_back(object.value("clip_size"));
    params.emplace_back(object.value("rate_of_fire"));
    params.emplace_back(object.value("reloading_time"));
    auto weapon_type = Constants::GetEnumValueFromString<WeaponType>(
        object.value("type").toString());
    weapon_params_by_type_[weapon_type] = std::move(params);
    json_object_by_type_[weapon_type] = object;
  }
}

std::vector<QVariant> WeaponSettings::GetParams(WeaponType weapon_type) const {
  return weapon_params_by_type_.at(weapon_type);
}
