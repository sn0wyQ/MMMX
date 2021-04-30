#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_

#include <map>
#include <vector>

#include <QJsonObject>
#include <QSizeF>

#include "constants.h"

class CreepSettings {
 public:
  static CreepSettings& GetInstance();

  std::vector<QVariant> GetCreepParams(float x, float y, float rotation,
                                       float distance_from_center) const;

  std::pair<int, int> GetMinAndMaxCreepLevelByDistance(float distance) const;

  QSizeF GetCreepSizeByLevel(int level) const;
  QSizeF GetMaxCreepSize() const;
  int GetCreepLevel(float distance_from_center) const;

  template<class T>
  T GetCreepSetting(const QString& setting_name) const {
    return json_object_.value(setting_name).toVariant().value<T>();
  }

 private:
  CreepSettings();
  static CreepSettings* instance_;
  QJsonObject json_object_;

  float CalculateExpIncrement(int level) const;
  float CalculateMaxHp(int level) const;
  float CalculateDefaultHp(int level) const;
  float CalculateRegenRate(int level) const;
  float CalculateWidth(int level) const;
  float CalculateHeight(int level) const;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
