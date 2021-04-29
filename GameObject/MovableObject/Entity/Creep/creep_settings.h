#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_

#include <unordered_map>
#include <vector>

#include "QJsonObject"

#include "constants.h"

class CreepSettings {
 public:
  static CreepSettings& GetInstance();

  std::vector<QVariant> GetCreepParams(int creep_level,
                                       float x,
                                       float y,
                                       float rotation) const;
  template<class T>
  T GetCreepSetting(int creep_level,
                    const QString& setting_name) const {
    return json_object_by_level_.at(creep_level).value(setting_name)
        .toVariant().value<T>();
  }

 private:
  CreepSettings();
  static CreepSettings* instance_;

  std::unordered_map<int, QJsonObject> json_object_by_level_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
