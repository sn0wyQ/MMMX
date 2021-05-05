#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <string>

#include <QCryptographicHash>
#include <QMetaEnum>
#include <QString>
#include <QUrl>

// code_readability++;
using ClientId = int;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Build Options
constexpr bool kClientEnableIgnoreLevel = true;
constexpr QtMsgType kClientMessageIgnoreLevel = QtDebugMsg;
constexpr bool kServerEnableIgnoreLevel = true;
constexpr QtMsgType kServerMessageIgnoreLevel = QtDebugMsg;

// Version
constexpr int kMajorVersion = 0;
constexpr int kMinorVersion = 1;
constexpr int kPatchVersion = 0;
const QString kVersionString = QString::number(kMajorVersion)
    + "." + QString::number(kMinorVersion)
    + "." + QString::number(kPatchVersion);

// Server
const QString kServerName = "MMMX Server";
constexpr int kServerPort = 1337;
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;
const auto kHashAlgorithm = QCryptographicHash::Algorithm::Md5;
constexpr int kMSecsToStore = 1000;
constexpr int64_t kTicksToStore = kMSecsToStore / kTimeToTick;

// Map
constexpr float kDefaultMapWidth = 100.f;
constexpr float kDefaultMapHeight = 100.f;

// Room
constexpr int kDefaultMaxClients = 15;

// Null Values
constexpr ClientId kNullClientId = -1;
constexpr GameObjectId kNullGameObjectId = -1;
constexpr RoomId kNullRoomId = -1;

// Client
const QString kWindowTitle = "MMMX | Version " + kVersionString;
constexpr int kTimeToUpdateVarsAndPing = 1000;
constexpr int64_t kInterpolationMSecs = 200;
constexpr int kShootHoldingCheck = 30;

// Graphics
constexpr float kBaseViewWidth = 25.f;
constexpr float kBaseViewHeight = 25.f;
constexpr float kPlayerBarHeightRatio = 0.15f;
constexpr bool kRigidBodyShow = true;
constexpr int kAverageFpsFrames = 50;

// Animations
constexpr int kUnloadAnimationCheckTime = 30;

// GameObject
constexpr float kDefaultMaxHealthPoints = 100.f;

// MovableObject
constexpr float kDefaultSpeedMultiplier = 0.01f;

// Player
constexpr float kDefaultEntityFov = 12.f;
constexpr float kDefaultPlayerRotation = 0.f;
constexpr float kDefaultPlayerRadius = 1.f;
constexpr float kDefaultHealthRegenSpeed = 0.002f;

// View
constexpr float kCameraStiffnessRatio = 0.008f;
constexpr float kCameraFrictionRatio = 0.15f;
constexpr float kFovStiffnessRatio = 0.007f;
constexpr float kFovFrictionRatio = 0.15f;
constexpr int kDefaultFpsMax = 60;

// Bullet collision
constexpr int kAccuracy = 10;

constexpr int kMaxLevel = 30;

constexpr float GetExpForLevel(int level) {
  return static_cast<float>(level) * 5.f;
}

constexpr float kExpMultiplier = 5.f;

constexpr int kUpgradeSlots = 10;
constexpr int kCountOfLevels = 5;

namespace LevelingMultipliers {

constexpr float kMaxHp = 1.5f;
constexpr float kHealthRegenRate = 2.1f;
constexpr float kSpeed = 1.1f;
constexpr float kFovRadius = 1.3f;

constexpr float kBulletSpeed = 1.2f;
constexpr float kRateOfFire = 1.1f;
constexpr float kBulletRange = 1.3f;
constexpr float kBulletDamage = 1.1f;

}  // namespace LevelingMultipliers

template<class T>
T GetEnumValueFromString(const QString& string) {
  auto weapon_type_index =
      QMetaEnum::fromType<T>().keyToValue(string.toLocal8Bit().constData());
  return static_cast<T>(weapon_type_index);
}

}  // namespace Constants

#endif  // CONSTANTS_H_
