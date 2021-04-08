#ifndef CONSTANTS_H_
#define CONSTANTS_H_

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

// Map
constexpr float kDefaultMapWidth = 50.f;
constexpr float kDefaultMapHeight = 50.f;

// Room
constexpr int kDefaultMaxClients = 4;

// Null Values
constexpr ClientId kNullClientId = -1;
constexpr GameObjectId kNullGameObjectId = -1;
constexpr RoomId kNullRoomId = -1;

// Client
const QString kWindowTitle = "MMMX | Version " + kVersionString;
constexpr int kTimeToUpdateVarsAndPing = 1000;

// Graphics
constexpr float kBaseViewWidth = 25.f;
constexpr float kBaseViewHeight = 25.f;
constexpr bool kRigidBodyShow = true;

// MovableObject
constexpr float kDefaultMovableObjectSpeed = 0.01f;

// Player
constexpr float kDefaultEntityFov = 10.5f;
constexpr float kDefaultPlayerX = 5.f;
constexpr float kDefaultPlayerY = 5.f;
constexpr float kDefaultPlayerRotation = 0.f;
constexpr float kDefaultPlayerRadius = 1.f;

}  // namespace Constants

#endif  // CONSTANTS_H_
