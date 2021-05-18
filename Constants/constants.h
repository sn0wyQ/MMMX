#ifndef CONSTANTS_CONSTANTS_H_
#define CONSTANTS_CONSTANTS_H_

#include <string>

#include <QCryptographicHash>
#include <QMetaEnum>
#include <QString>
#include <QPainter>
#include <QUrl>

// code_readability++;
using ClientId = int;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Build Options
extern const bool kClientEnableIgnoreLevel;
extern const QtMsgType kClientMessageIgnoreLevel;
extern const bool kServerEnableIgnoreLevel;
extern const QtMsgType kServerMessageIgnoreLevel;

// Version
extern const int kMajorVersion;
extern const int kMinorVersion;
extern const int kPatchVersion;
const QString kVersionString = QString::number(kMajorVersion)
    + "." + QString::number(kMinorVersion)
    + "." + QString::number(kPatchVersion);

// Server
const QString kServerName = "MMMX Server";
extern const int kServerPort;
extern const int kTickrate;
extern const int kTimeToTick;
extern const QCryptographicHash::Algorithm kHashAlgorithm;
extern const int kMSecsToStore;
extern const int64_t kTicksToStore;

// Map
extern const float kDefaultMapWidth;
extern const float kDefaultMapHeight;

// Room
extern const int kDefaultMaxClients;

// Null Values
extern const ClientId kNullClientId;
extern const GameObjectId kNullGameObjectId;
extern const RoomId kNullRoomId;

// Client
const QString kWindowTitle = "MMMX | Version " + kVersionString;
extern const int kTimeToUpdateVarsAndPing;
extern const int64_t kInterpolationMSecs;
extern const int kControlsHoldingCheck;

// Graphics
extern const float kBaseViewWidth;
extern const float kBaseViewHeight;
extern const float kPlayerBarHeightRatio;
extern const bool kRigidBodyShow;
extern const int kAverageFpsFrames;

// Animations
extern const int kUnloadAnimationCheckTime;

// MovableObject
extern const float kDefaultSpeedMultiplier;

// Entity
extern const float kDefaultMaxHealthPoints;

// Player
extern const float kDefaultEntityFov;
extern const float kDefaultPlayerRotation;
extern const float kDefaultPlayerRadius;
extern const float kDefaultHealthRegenSpeed;
extern const int kMaxLevel;

// Revive
extern const int64_t kReviveTime;
extern const int64_t kRequestRespawnTime;
extern const int64_t kHoldingRespawnTime;

// View
void SetPainterHints(QPainter* painter);
extern const float kCameraStiffnessRatio;
extern const float kCameraFrictionRatio;
extern const float kFovStiffnessRatio;
extern const float kFovFrictionRatio;
extern const int kDefaultFpsMax;

// Bullet collision
extern const int kAccuracy;

float GetExpForLevel(int level);
extern const float kExpMultiplier;

extern const int kUpgradeSlots;
extern const int kCountOfLevels;

namespace LevelingMultipliers {

extern const float kMaxHp;
extern const float kHealthRegenRate;
extern const float kSpeed;
extern const float kFovRadius;
extern const float kAccuracy;

extern const float kBulletSpeed;
extern const float kRateOfFire;
extern const float kBulletRange;
extern const float kBulletDamage;
extern const float kReloadingTime;

}  // namespace LevelingMultipliers

template<class T>
T GetEnumValueFromString(const QString& string) {
  auto weapon_type_index =
      QMetaEnum::fromType<T>().keyToValue(string.toLocal8Bit().constData());
  return static_cast<T>(weapon_type_index);
}

template<class T>
QString GetStringFromEnumValue(T value) {
  return QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value));
}

extern const int kMaxPlayerBullets;

}  // namespace Constants

#endif  // CONSTANTS_CONSTANTS_H_