#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <QString>
#include <QUrl>

namespace Constants {

// Version:
constexpr int kMajorVersion = 0;
constexpr int kMinorVersion = 0;
constexpr int kBuildNumber = 1;
const QString kVersionString = "0.0.1";

// Server info:
constexpr quint16 kServerPort = 1337;
const QUrl server = QUrl("188.120.224.70");
constexpr int kTickrate = 64;
constexpr int kTimeToTick = 1000 / kTickrate;

}  // namespace Constants

#endif  // CONSTANTS_H_
