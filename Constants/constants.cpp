#include "Constants/constants.h"

// code_readability++;
using ClientId = int;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Build Options
const bool kClientEnableIgnoreLevel = true;
const QtMsgType kClientMessageIgnoreLevel = QtDebugMsg;
const bool kServerEnableIgnoreLevel = true;
const QtMsgType kServerMessageIgnoreLevel = QtDebugMsg;

// Version
const int kMajorVersion = 0;
const int kMinorVersion = 1;
const int kPatchVersion = 0;

// Server
const int kServerPort = 1337;
const int kTickrate = 64;
const int kTimeToTick = 1000 / kTickrate;
const QCryptographicHash::Algorithm kHashAlgorithm =
    QCryptographicHash::Algorithm::Md5;
const int kMSecsToStore = 1000;
const int64_t kTicksToStore = kMSecsToStore / kTimeToTick;

// Map
const float kMapWidth = 100.f;
const float kMapHeight = 100.f;

// Room
const int kDefaultMaxClients = 15;

// Null Values
const ClientId kNullClientId = -1;
const GameObjectId kNullGameObjectId = -1;
const RoomId kNullRoomId = -1;

// Client
const int kTimeToUpdateVarsAndPing = 1000;
const int64_t kInterpolationMSecs = 200;
const int kControlsHoldingCheck = 30;

// Graphics
const int kAntiAliasingSamples = 5;
const float kBaseViewWidth = 25.f;
const float kBaseViewHeight = 25.f;
const float kPlayerBarHeightRatio = 0.15f;
const bool kRigidBodyShow = true;
const int kAverageFpsFrames = 50;

// Animations
const int kUnloadAnimationCheckTime = 30;

// MovableObject
const float kDefaultSpeedMultiplier = 0.01f;

// Entity
const int64_t kMaxDisappearTimeMSecs = 2000.f;
const float kOpacityChangeSpeed = 0.01f;
const float kDefaultMaxHealthPoints = 100.f;

// Player
const float kDefaultEntityFov = 12.f;
const float kDefaultPlayerRotation = 0.f;
const float kDefaultPlayerRadius = 1.f;
const float kDefaultHealthRegenSpeed = 0.002f;
const int kMaxLevel = 30;

// Revive
const int64_t kReviveTime = 5000;
const int64_t kRequestRespawnTime = 10000;
const int64_t kHoldingRespawnTime = 1000;

// Spawn settings
const int kChunksX = 15;
const int kChunksY = 15;
// The less this constant is the more creeps will spawn in center
const int kCreepSpawnShuffleRatio = 3;
const int kGenerationTries = 15;

// View
QColor GetHealthPointsColor(float hp_ratio) {
  QColor color;
  color.setHslF(0.25 * hp_ratio, 1.f, 0.5f);
  return color;
}
void SetPainterHints(QPainter* painter) {
  painter->setRenderHints(
      QPainter::Antialiasing |
      QPainter::SmoothPixmapTransform|
      QPainter::TextAntialiasing);
}
const float kCameraStiffnessRatio = 0.008f;
const float kCameraFrictionRatio = 0.15f;
const float kFovStiffnessRatio = 0.007f;
const float kFovFrictionRatio = 0.15f;
const int kDefaultFpsMax = 60;

// Bullet collision
const int kAccuracy = 10;

float GetExpForLevel(int level) {
  return static_cast<float>(level) * 5.f;
}

const float kExpMultiplier = 5.f;

const int kCountOfLevels = 5;

namespace LevelingSlots {

const float kMaxHp = 1.5f;
const float kHealthRegenRate = 2.1f;
const float kSpeed = 1.1f;
const float kFovRadius = 1.2f;
const float kAccuracy = 0.9f;

const float kBulletSpeed = 1.2f;
const float kRateOfFire = 1.1f;
const float kBulletRange = 1.3f;
const float kBulletDamage = 1.1f;
const float kReloadingTime = 0.9f;

}  // namespace LevelingSlots

const int kMaxPlayerBullets = 100000;

}  // namespace Constants
