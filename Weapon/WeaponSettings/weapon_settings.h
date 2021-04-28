#ifndef WEAPON_WEAPONSETTINGS_WEAPON_SETTINGS_H_
#define WEAPON_WEAPONSETTINGS_WEAPON_SETTINGS_H_

#include <QtCore>

#include "constants.h"

namespace WeaponTypeWrapper {

Q_NAMESPACE

enum class WeaponType {
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
  T GetWeaponSetting(WeaponType weapon_type, QString setting_name) const {
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
