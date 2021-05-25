#ifndef PLAYER_ENUMS_H_
#define PLAYER_ENUMS_H_

#include <QMetaObject>
#include <QMetaType>

namespace PlayerEnumsWrapper {

Q_NAMESPACE

enum class LevelingSlots {
  kMaxHp,
  kHealthRegenRate,
  kSpeed,
  kFovRadius,
  kAccuracy,
  kBulletSpeed,
  kRateOfFire,
  kBulletRange,
  kBulletDamage,
  kReloadingTime,
  SIZE
};

Q_ENUM_NS(LevelingSlots)

enum class PlayerType {
  kNone = 0,  // MUST stay 0
  kSmasher,
  kSoldier,
  kSpider,
  kViking
};

Q_ENUM_NS(PlayerType)

}  // namespace PlayerEnumsWrapper

using LevelingSlots = PlayerEnumsWrapper::LevelingSlots;
Q_DECLARE_METATYPE(LevelingSlots)

using PlayerType = PlayerEnumsWrapper::PlayerType;
Q_DECLARE_METATYPE(PlayerType)

#endif  // PLAYER_ENUMS_H_
