#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <QString>
#include <QUrl>

#include "defines.h"

// code_readability++;
using ClientId = int;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Version
constexpr int kMajorVersion = 0;
constexpr int kMinorVersion = 0;
constexpr int kBuildNumber = 1;
const QString kVersionString = QString::number(kMajorVersion)
    + "." + QString::number(kMinorVersion)
    + "." + QString::number(kBuildNumber);

// Server
const QString kServerName = "MMMX Server";
constexpr int kServerPort = 1337;
#if ONLINE
const QString kServerIp = "188.120.224.70";
#else
const QString kServerIp = "localhost";
#endif
const QUrl kServerUrl =
    QUrl(QString("ws://") + kServerIp + ":" + QString::number(kServerPort));
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;

// Room
constexpr int kDefaultMaxClients = 4;

// Null Values
constexpr ClientId kNullClientId = -1;
constexpr GameObjectId kNullGameObjectId = -1;
constexpr RoomId kNullRoomId = -1;

// Client
const QString kWindowTitle = "MMMX | Version " + kVersionString;
constexpr int kTimeToUpdatePing = 1000;
constexpr int kTimeToUpdateServerVar = 1000;
constexpr int kTimeToUpdateControls = 5;

// Graphics
constexpr float kBaseViewWidth = 25.f;
constexpr float kBaseViewHeight = 25.f;

// Player
constexpr float kDefaultPlayerX = 5.f;
constexpr float kDefaultPlayerY = 5.f;
constexpr float kDefaultPlayerViewAngle = 0.f;

}  // namespace Constants

#endif  // CONSTANTS_H_
