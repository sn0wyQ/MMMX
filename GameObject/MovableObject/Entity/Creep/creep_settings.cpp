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
    qWarning() << "[CREEP SETTINGS] Json document cannot be open";
  }

  QJsonObject json_object = QJsonDocument::fromJson(file.readAll()).object();

  QJsonValue json_value = json_object.value("distribution_delta");
  if (!json_value.isDouble()) {
    qWarning() << "[CREEP SETTINGS] Json document has wrong format";
  } else {
    distribution_delta_ = static_cast<float>(json_value.toDouble());
  }

  json_value = json_object.value("distribution_lambda");
  if (!json_value.isDouble()) {
    qWarning() << "[CREEP SETTINGS] Json document has wrong format";
  } else {
    distribution_lambda_ = static_cast<float>(json_value.toDouble());
  }

  json_value = json_object.value("max_width");
  if (!json_value.isDouble()) {
    qWarning() << "[CREEP SETTINGS] Json document has wrong format";
  } else {
    max_creep_size_.setWidth(json_value.toDouble());
  }

  json_value = json_object.value("max_height");
  if (!json_value.isDouble()) {
    qWarning() << "[CREEP SETTINGS] Json document has wrong format";
  } else {
    max_creep_size_.setHeight(json_value.toDouble());
  }

  json_value = json_object.value("creep_params_array");
  if (!json_value.isArray()) {
    qWarning() << "[CREEP SETTINGS] Json document has wrong format";
  } else {
    QJsonArray creeps_params = json_value.toArray();
    for (const auto& creep_params : creeps_params) {
      if (!creep_params.isObject()) {
        qWarning() << "[CREEP SETTINGS] Json document has wrong format";
      } else {
        QVariantMap
            current_creep_params = creep_params.toObject().toVariantMap();
        QVariant current_creep_type = current_creep_params.value("type");
        if (!current_creep_type.isValid()
            || !current_creep_type.canConvert<QString>()) {
          qWarning() << "[CREEP SETTINGS] Json document has wrong format";
        } else {
          bool found_min_level;
          int min_level =
              current_creep_params.value("min_level").toInt(&found_min_level);
          bool found_max_level;
          int max_level =
              current_creep_params.value("max_level").toInt(&found_max_level);
          bool found_base_xp;
          float base_xp =
              current_creep_params.value("base_xp").toFloat(&found_base_xp);
          bool found_xp_multiplier;
          float xp_multiplier =
              current_creep_params.value("xp_multiplier")
                  .toFloat(&found_xp_multiplier);
          if (!found_min_level || !found_max_level || !found_base_xp
              || !found_xp_multiplier) {
            qWarning() << "[CREEP SETTINGS] Json document has wrong format";
          } else {
            float min_level_xp = std::pow(xp_multiplier, min_level) * base_xp;
            current_creep_params.insert("min_level_xp", min_level_xp);
            float max_level_xp = std::pow(xp_multiplier, max_level) * base_xp;
            current_creep_params.insert("max_level_xp", max_level_xp);
            min_creep_xp_ = std::min(min_creep_xp_, min_level_xp);
            max_creep_xp_ = std::max(max_creep_xp_, max_level_xp);
            creeps_params_.insert(Constants::GetEnumValueFromString<CreepType>(
                                      current_creep_type.toString()),
                                  current_creep_params);
          }
        }
      }
    }
  }
}

std::pair<int, CreepType>
    CreepSettings::GenerateCreepLevelAndType(float distance_from_center) const {
  static std::mt19937 gen(QDateTime::currentMSecsSinceEpoch());
  std::uniform_real_distribution<float>
      real_distribution(0.f, distribution_delta_ * 2.f);
  // TODO(Someone): rework for something more... equally distributed?
  float
      normalized = distance_from_center / Constants::kMapMaxDistanceFromCenter;
  float distributed = -std::log(normalized) / distribution_lambda_;
  float suitable_xp = distributed
                      + (real_distribution(gen) - distribution_delta_);
  suitable_xp *= max_creep_xp_;
  suitable_xp = std::clamp(suitable_xp, min_creep_xp_, max_creep_xp_);

  std::vector<std::pair<int, CreepType>> suitable_pairs;
  for (const auto& creep_params : creeps_params_) {
    auto creep_type = Constants::GetEnumValueFromString<CreepType>(
                              creep_params.value("type").toString());
    auto min_level_xp = GetCreepSetting<float>(creep_type, "min_level_xp");
    auto max_level_xp = GetCreepSetting<float>(creep_type, "max_level_xp");
    if (min_level_xp <= suitable_xp && suitable_xp <= max_level_xp) {
      int best_lvl = GetCreepSetting<int>(creep_type, "min_level");
      float best_xp_diff = std::abs(GetXp(creep_type, best_lvl)
                                    - suitable_xp);
      int cur_lvl = best_lvl + 1;
      int max_lvl = GetCreepSetting<int>(creep_type, "max_level");
      while (cur_lvl <= max_lvl) {
        float cur_xp_diff = std::abs(GetXp(creep_type, cur_lvl)
                                     - suitable_xp);
        if (best_xp_diff > cur_xp_diff) {
          best_xp_diff = cur_xp_diff;
          best_lvl = cur_lvl;
        }
        ++cur_lvl;
      }

      suitable_pairs.emplace_back(best_lvl, creep_type);
    }
  }

  if (suitable_pairs.empty()) {
    qWarning() << "[CREEP SETTINGS] Can not find suitable Creep for distance"
               << distance_from_center;
    return { 3, CreepType::kBox };
  }

  std::uniform_int_distribution<int>
      final_distribution(0, static_cast<int>(suitable_pairs.size() - 1));
  return suitable_pairs.at(final_distribution(gen));
}

std::vector<QVariant>
     CreepSettings::GetCreepParams(float x, float y,
                                   float distance_from_center) const {
  std::vector<QVariant> params;
  auto level_and_type = GenerateCreepLevelAndType(distance_from_center);
  if (level_and_type.second == CreepType::kNone) {
    return {};
  }

  auto rotation =
      static_cast<float>(QRandomGenerator::global()->bounded(360.f));
  float max_hp = GetMaxHp(level_and_type.second, level_and_type.first);

  // GameObject params
  params.emplace_back(x);
  params.emplace_back(y);
  params.emplace_back(rotation);
  params.emplace_back(GetWidth(level_and_type.second));
  params.emplace_back(GetHeight(level_and_type.second));
  params.emplace_back(
      static_cast<int>(GetRigidBodyType(level_and_type.second)));
  params.emplace_back(GetRigidBodyWidth(level_and_type.second));
  params.emplace_back(GetRigidBodyHeight(level_and_type.second));
  params.emplace_back(
      static_cast<int>(GetAnimationType(level_and_type.second)));
  params.emplace_back(true);
  // MovableObject params
  params.emplace_back(0);  // velocity_x
  params.emplace_back(0);  // velocity_y
  params.emplace_back(GetSpeed(level_and_type.second, level_and_type.first));
  // Entity params
  params.emplace_back(GetFov(level_and_type.second));
  params.emplace_back(max_hp);
  params.emplace_back(GetRegenRate(level_and_type.second,
                                   level_and_type.first));
  params.emplace_back(max_hp);
  // Creep params
  params.emplace_back(level_and_type.first);
  params.emplace_back(static_cast<int>(level_and_type.second));
  params.emplace_back(GetXp(level_and_type.second, level_and_type.first));
  return params;
}

float CreepSettings::GetXp(CreepType creep_type, int level) const {
  auto base_xp = GetCreepSetting<float>(creep_type, "base_xp");
  auto xp_multiplier = GetCreepSetting<float>(creep_type, "xp_multiplier");
  return std::pow(xp_multiplier, level - 1) * base_xp;
}

float CreepSettings::GetWidth(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "width");
}

float CreepSettings::GetHeight(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "height");
}

RigidBodyType CreepSettings::GetRigidBodyType(CreepType creep_type) const {
  return Constants::GetEnumValueFromString<RigidBodyType>(
      GetCreepSetting<QString>(creep_type, "rigid_body_type"));
}

float CreepSettings::GetRigidBodyWidth(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "rigid_body_width");
}

float CreepSettings::GetRigidBodyHeight(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "rigid_body_height");
}

AnimationType CreepSettings::GetAnimationType(CreepType creep_type) const {
  return Constants::GetEnumValueFromString<AnimationType>(
      GetCreepSetting<QString>(creep_type, "animation"));
}

float CreepSettings::GetSpeed(CreepType creep_type, int level) const {
  auto base_speed = GetCreepSetting<float>(creep_type, "base_speed");
  auto
      speed_multiplier = GetCreepSetting<float>(creep_type, "speed_multiplier");
  return std::pow(speed_multiplier, level - 1) * base_speed;
}

float CreepSettings::GetFov(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "fov");
}

float CreepSettings::GetMaxHp(CreepType creep_type, int level) const {
  auto base_max_hp = GetCreepSetting<float>(creep_type, "base_max_hp");
  auto max_hp_multiplier =
      GetCreepSetting<float>(creep_type, "max_hp_multiplier");
  return std::pow(max_hp_multiplier, level - 1) * base_max_hp;
}

float CreepSettings::GetRegenRate(CreepType creep_type, int level) const {
  auto base_hp_regen_rate =
      GetCreepSetting<float>(creep_type, "base_hp_regen_rate");
  auto hp_regen_rate_multiplier =
      GetCreepSetting<float>(creep_type, "hp_regen_rate_multiplier");
  return std::pow(hp_regen_rate_multiplier, level - 1) * base_hp_regen_rate;
}

float CreepSettings::GetDamage(CreepType creep_type, int level) const {
  auto base_damage = GetCreepSetting<float>(creep_type, "base_damage");
  auto damage_multiplier =
      GetCreepSetting<float>(creep_type, "damage_multiplier");
  return std::pow(damage_multiplier, level - 1) * base_damage;
}

float CreepSettings::GetAttackDistance(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "attack_distance");
}

int64_t CreepSettings::GetReloadingTime(CreepType creep_type) const {
  return GetCreepSetting<int64_t>(creep_type, "reloading_time");
}

float CreepSettings::GetRunawayHpRatio(CreepType creep_type) const {
  return GetCreepSetting<float>(creep_type, "runaway_hp_ratio");
}

bool CreepSettings::HasIntellect(CreepType creep_type) const {
  return GetCreepSetting<bool>(creep_type, "has_intellect");
}

QSizeF CreepSettings::GetMaxCreepSize() const {
  return max_creep_size_;
}

void CreepSettings::SetStaticParams(const std::shared_ptr<Creep>& creep) {
  creep->SetSpawnX(creep->GetX());
  creep->SetSpawnY(creep->GetY());
  CreepType creep_type = creep->GetCreepType();
  int creep_level = creep->GetLevel();
  creep->SetFovRadius(CreepSettings::GetInstance().GetFov(creep_type));
  creep->SetAttackDistance(
      CreepSettings::GetInstance().GetAttackDistance(creep_type));
  creep->SetDamage(
      CreepSettings::GetInstance().GetDamage(creep_type, creep_level));
  creep->SetReloadingTime(
      CreepSettings::GetInstance().GetReloadingTime(creep_type));
}
