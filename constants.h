#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <QString>
#include <QUrl>

// code_readability++;
using ClientId = int;
using PlayerNickname = QString;
using GameObjectId = int;
using RoomId = int;

namespace Constants {

// Version
constexpr int kMajorVersion = 0;
constexpr int kMinorVersion = 0;
constexpr int kBuildNumber = 1;
const QString kVersionString = "0.0.1";

// Server
const QString kServerName = "MMMX Server";
constexpr int kServerPort = 1337;
// const QString kServerIp = QString("ws://") + "188.120.224.70";
const QString kServerIp = QString("ws://") + "localhost";
const QUrl kServerUrl = QUrl(kServerIp + ":" + QString::number(kServerPort));
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;

// Room
constexpr int kDefaultMaxClients = 2;

// Null Values
constexpr ClientId kNullClientId = -1;
constexpr GameObjectId kNullGameObjectId = -1;
constexpr RoomId kNullRoomId = -1;

// Client
const QString kWindowTitle = "MMMX | Version " + kVersionString;

}  // namespace Constants

#endif  // CONSTANTS_H_
