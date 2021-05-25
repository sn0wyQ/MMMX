#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineF>
#include <QRandomGenerator>
#include <QSizeF>
#include <QVariantMap>

#include "Animation/animation_enums.h"
#include "Constants/constants.h"
#include "GameObject/MovableObject/Entity/Creep/creep.h"
#include "GameObject/MovableObject/Entity/Creep/creep_enums.h"
#include "GameObject/RigidBody/rigid_body.h"

class CreepSettings {
 public:
  static CreepSettings& GetInstance();

  std::vector<QVariant> GetCreepParams(float x, float y,
                                       float distance_from_center) const;

  QSizeF GetMaxCreepSize() const;
  std::pair<int, CreepType>
      GenerateCreepLevelAndType(float distance_from_center) const;

  template<class T>
  T GetCreepSetting(CreepType creep_type, const QString& setting_name) const {
    auto creep_iter = creeps_params_.constFind(creep_type);
    if (creep_iter == creeps_params_.end()) {
      qWarning() << "[CREEP SETTINGS] No such creep type:" << creep_type;
      return T();
    }

    QVariantMap::const_iterator
        setting_iter = creep_iter->constFind(setting_name);
    if (setting_iter == creep_iter->end()) {
      qWarning() << "[CREEP SETTINGS] No such setting name:" << setting_name;
      return T();
    }

    if (!setting_iter->canConvert<T>()) {
      qWarning() << "[CREEP SETTINGS] Can not convert:" << setting_name
                 << "to type" << QMetaType::fromType<T>().name();
    }
    return setting_iter->value<T>();
  }

  float GetExp(CreepType creep_type, int level) const;
  float GetWidth(CreepType creep_type) const;
  float GetHeight(CreepType creep_type) const;
  RigidBodyType GetRigidBodyType(CreepType creep_type) const;
  float GetRigidBodyWidth(CreepType creep_type) const;
  float GetRigidBodyHeight(CreepType creep_type) const;
  AnimationType GetAnimationType(CreepType creep_type) const;
  float GetSpeed(CreepType creep_type, int level) const;
  float GetFov(CreepType creep_type) const;
  float GetMaxHp(CreepType creep_type, int level) const;
  float GetRegenRate(CreepType creep_type, int level) const;
  float GetDamage(CreepType creep_type, int level) const;
  float GetAttackDistance(CreepType creep_type) const;
  int64_t GetReloadingTime(CreepType creep_type) const;
  float GetRunawayHpRatio(CreepType creep_type) const;
  bool HasIntelligence(CreepType creep_type) const;

  static void SetStaticParams(const std::shared_ptr<Creep>& creep);

 private:
  CreepSettings();
  static CreepSettings* instance_;

  QMap<CreepType, QVariantMap> creeps_params_;
  float min_creep_xp_{std::numeric_limits<float>::max()};
  float max_creep_xp_{std::numeric_limits<float>::min()};
  float distribution_delta_{};
  float distribution_lambda_{};
  QSizeF max_creep_size_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_SETTINGS_H_
