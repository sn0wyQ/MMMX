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
constexpr quint16 kServerPort = 1337;
const QUrl kServerUrl = QUrl("ws://188.120.224.70:1337");
// const QUrl kServerUrl = QUrl("ws://localhost:1337");
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;

// Room
// constexpr int kDefaultMaxClients = 4;
constexpr int kDefaultMaxClients = 2;

// Null Values
constexpr ClientId kNullClientId = -1;
constexpr GameObjectId kNullGameObjectId = -1;
constexpr RoomId kNullRoomId = -1;

}  // namespace Constants

#endif  // CONSTANTS_H_
