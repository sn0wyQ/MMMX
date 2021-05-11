#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QRandomGenerator>
#include <QJsonObject>
#include <QSizeF>

#include "Animation/animation_enums.h"
#include "GameObject/MovableObject/Entity/Creep/creep.h"
#include "GameObject/MovableObject/Entity/Creep/creep_enums.h"
#include "constants.h"

class CreepSettings {
 public:
  static CreepSettings& GetInstance();

  std::vector<QVariant> GetCreepParams(float x, float y,
                                       float distance_from_center) const;

  std::pair<int, int> GetMinAndMaxCreepLevelByDistance(float distance) const;

  QSizeF GetCreepSizeByLevel(int level) const;
  QSizeF GetMaxCreepSize() const;
  int GetCreepLevel(float distance_from_center) const;

  template<class T>
  T GetCreepSetting(const QString& setting_name) const {
    if (!json_object_.contains(setting_name)) {
      qWarning() << "No such setting name:" << setting_name;
    }
    return json_object_.value(setting_name).toVariant().value<T>();
  }

  float CalculateFov(int level) const;
  float CalculateDamage(int level) const;

  static void SetStaticParams(const std::shared_ptr<Creep>& creep);

 private:
  CreepSettings();
  static CreepSettings* instance_;

  QJsonObject json_object_;
  float CalculateExpIncrement(int level) const;
  float CalculateMaxHp(int level) const;
  float CalculateRegenRate(int level) const;
  float CalculateWidth(int level) const;
  float CalculateHeight(int level) const;
  float CalculateSpeed(int level) const;
  CreepType CalculateType(int level) const;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
