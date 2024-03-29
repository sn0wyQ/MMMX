#include "creep_settings.h"

CreepSettings* CreepSettings::instance_{nullptr};

CreepSettings& CreepSettings::GetInstance() {
  if (!instance_) {
    try {
      instance_ = new CreepSettings();
    } catch (const std::exception& ex) {
      qWarning() << ex.what();
    }
  }
  return *instance_;
}

float LoadFloat(const QJsonObject& json_object, const QString& key) {
  QJsonValue json_value = json_object.value(key);
  if (!json_value.isDouble()) {
    throw std::runtime_error("[CREEP SETTINGS] Json document has wrong format");
  }

  return static_cast<float>(json_value.toDouble());
}

template<typename T>
T GetValueFromVariantMap(const QVariantMap& map, const QString& key) {
  QVariant variant = map.value(key);
  if (!variant.canConvert<T>()) {
    throw std::runtime_error("[CREEP SETTINGS] Json document has wrong format");
  }

  return variant.value<T>();
}

CreepSettings::CreepSettings() {
  QFile file(":creep_settings.json");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "[CREEP SETTINGS] Json document cannot be open";
  }

  QJsonObject json_object = QJsonDocument::fromJson(file.readAll()).object();

  distribution_delta_ = LoadFloat(json_object, "distribution_delta");
  distribution_lambda_ = LoadFloat(json_object, "distribution_lambda");
  max_creep_size_.setWidth(LoadFloat(json_object, "max_width"));
  max_creep_size_.setHeight(LoadFloat(json_object, "max_height"));

  QJsonValue json_value = json_object.value("creep_params_array");
  if (!json_value.isArray()) {
    throw std::runtime_error("[CREEP SETTINGS] Json document has wrong format");
  }

  QJsonArray creeps_params = json_value.toArray();
  for (const auto& creep_params : creeps_params) {
    if (!creep_params.isObject()) {
      throw std::runtime_error(
          "[CREEP SETTINGS] Json document has wrong format");
    }

    auto current_creep_params = creep_params.toObject().toVariantMap();
    QVariant current_creep_type = current_creep_params.value("type");
    if (!current_creep_type.isValid()
        || !current_creep_type.canConvert<QString>()) {
      throw std::runtime_error(
          "[CREEP SETTINGS] Json document has wrong format");
    }

    auto min_level =
        GetValueFromVariantMap<int>(current_creep_params, "min_level");
    auto max_level =
        GetValueFromVariantMap<int>(current_creep_params, "max_level");
    float base_exp =
        GetValueFromVariantMap<float>(current_creep_params, "base_exp");
    float xp_multiplier =
        GetValueFromVariantMap<float>(current_creep_params,
                                      "xp_multiplier");

    float min_level_xp = std::pow(xp_multiplier, min_level) * base_exp;
    current_creep_params.insert("min_level_xp", min_level_xp);
    float max_level_xp = std::pow(xp_multiplier, max_level) * base_exp;
    current_creep_params.insert("max_level_xp", max_level_xp);
    min_creep_xp_ = std::min(min_creep_xp_, min_level_xp);
    max_creep_xp_ = std::max(max_creep_xp_, max_level_xp);
    creeps_params_.insert(Constants::GetEnumValueFromString<CreepType>(
                              current_creep_type.toString()),
                          current_creep_params);
  }
}

std::pair<int, CreepType>
    CreepSettings::GenerateCreepLevelAndType(float distance_from_center) const {
  static std::mt19937 gen(QDateTime::currentMSecsSinceEpoch());
  std::uniform_real_distribution<float>
      real_distribution(0.f, distribution_delta_ * 2.f);
  float
      normalized = distance_from_center /
          Math::DistanceBetweenPoints(QPointF(),
                                      QPointF(Constants::kMapWidth / 2.f,
                                              Constants::kMapHeight / 2.f));
  float distributed = -std::log(normalized) / distribution_lambda_;
  float suitable_xp = distributed
                      + (real_distribution(gen) - distribution_delta_);
  suitable_xp *= max_creep_xp_;
  suitable_xp = std::clamp(suitable_xp, min_creep_xp_, max_creep_xp_);

  std::vector<std::pair<int, CreepType>> suitable_pairs;
  for (const auto& creep_params : creeps_params_) {
    auto creep_type = Constants::GetEnumValueFromString<CreepType>(
                              creep_params.value("type").toString());
    auto min_suitable_xp =
        GetCreepSetting<float>(creep_type, "min_level_xp")
        / GetCreepSetting<float>(creep_type, "xp_multiplier");
    auto max_suitable_xp =
        GetCreepSetting<float>(creep_type, "max_level_xp")
        * GetCreepSetting<float>(creep_type, "xp_multiplier");
    if (min_suitable_xp <= suitable_xp && suitable_xp <= max_suitable_xp) {
      int best_lvl = GetCreepSetting<int>(creep_type, "min_level");
      float best_xp_diff = std::abs(GetExp(creep_type, best_lvl)
                                    - suitable_xp);
      int cur_lvl = best_lvl + 1;
      int max_lvl = GetCreepSetting<int>(creep_type, "max_level");
      while (cur_lvl <= max_lvl) {
        float cur_xp_diff = std::abs(GetExp(creep_type, cur_lvl)
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
               << distance_from_center << ", suitable xp is" << suitable_xp;
    return { 3, CreepType::kBox };
  }

  return suitable_pairs.at(gen() % suitable_pairs.size());
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
  params.emplace_back(GetExp(level_and_type.second, level_and_type.first));
  return params;
}

float CreepSettings::GetExp(CreepType creep_type, int level) const {
  auto base_exp = GetCreepSetting<float>(creep_type, "base_exp");
  auto xp_multiplier = GetCreepSetting<float>(creep_type, "xp_multiplier");
  return std::pow(xp_multiplier, level - 1) * base_exp;
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
// TODO(Everybody) Creep speed
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

bool CreepSettings::HasIntelligence(CreepType creep_type) const {
  return GetCreepSetting<bool>(creep_type, "has_intelligence");
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
