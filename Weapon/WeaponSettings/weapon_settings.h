#ifndef WEAPON_WEAPONSETTINGS_WEAPON_SETTINGS_H_
#define WEAPON_WEAPONSETTINGS_WEAPON_SETTINGS_H_

#include <unordered_map>
#include <vector>

#include <QMetaEnum>
#include "QJsonObject"

#include "constants.h"

namespace WeaponTypeWrapper {

Q_NAMESPACE

enum class WeaponType {
  kNull = -1,
  kAssaultRifle,
  kCrossbow,
  kMachineGun,
  kShotgun,

  SIZE
};

Q_ENUM_NS(WeaponType)

}  // namespace WeaponTypeWrapper

using WeaponType = WeaponTypeWrapper::WeaponType;

Q_DECLARE_METATYPE(WeaponType)

class WeaponSettings {
 public:
  static WeaponSettings& GetInstance();

  std::vector<QVariant> GetParams(WeaponType weapon_type) const;
  template<class T>
  T GetWeaponSetting(WeaponType weapon_type,
                     const QString& setting_name) const {
    if (!json_object_by_type_.at(weapon_type).contains(setting_name)) {
      qWarning() << "No such setting name:" << setting_name;
    }
    return json_object_by_type_.at(weapon_type).value(setting_name)
        .toVariant().value<T>();
  }

 private:
  WeaponSettings();
  static WeaponSettings* instance_;

  std::unordered_map<WeaponType, std::vector<QVariant>>
      weapon_params_by_type_;
  std::unordered_map<WeaponType, QJsonObject> json_object_by_type_;
};

#endif  // WEAPON_WEAPONSETTINGS_WEAPON_SETTINGS_H_
