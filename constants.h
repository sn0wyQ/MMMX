#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <QCryptographicHash>
#include <QString>
#include <QUrl>

// code_readability++;
using ClientId = int;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Build Options
constexpr bool kRemote = false;
constexpr bool kServerShowOnlyInfoMessages = false;
constexpr bool kServerShowDebugMessagesOnScreen = false;
constexpr bool kClientShowOnlyInfoMessages = false;
constexpr bool kClientShowDebugMessagesOnScreen = false;

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
const QString kServerIp = kRemote ? "188.120.224.70" : "localhost";
const QUrl kServerUrl =
    QUrl(QString("ws://") + kServerIp + ":" + QString::number(kServerPort));
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;
const auto kHashAlgorithm = QCryptographicHash::Algorithm::Md5;
constexpr int kMSecsToStore = 1000;
constexpr int64_t kTicksToStore = kMSecsToStore / kTimeToTick;
constexpr float kFovMultiplier = 1.7f;

// Map
constexpr float kDefaultMapWidth = 100.f;
constexpr float kDefaultMapHeight = 100.f;

// Room
constexpr int kDefaultMaxClients = 4;

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
constexpr bool kRigidBodyShow = true;

// GameObject
constexpr float kDefaultMaxHealthPoints = 100.f;

// MovableObject
constexpr float kDefaultMovableObjectSpeed = 0.01f;

// Player
constexpr float kDefaultEntityFov = 12.f;
constexpr float kDefaultPlayerRotation = 0.f;
constexpr float kDefaultPlayerRadius = 1.f;
constexpr float kDefaultHealthRegenSpeed = 0.001f;

// Bullet collision
constexpr int kAccuracy = 10;

constexpr float kExpForLevel[] = {
    10, 15, 20, 25, 30, 35, 40, 45, 50, 55
};
constexpr float kExpMultiplier = 5.f;

}  // namespace Constants

#endif  // CONSTANTS_H_
