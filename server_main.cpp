#include <memory>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTime>

#include "Server/server_controller.h"

void ServerMessageHandler(QtMsgType type,
                          const QMessageLogContext& context,
                          const QString& message) {
  if constexpr (kServerShowOnlyInfoMessages) {
    if (type != QtInfoMsg) {
      return;
    }
  }

  QString txt;
  switch (type) {
    case QtDebugMsg:
      txt = QString("Debug: %1").arg(message);
      break;

    case QtWarningMsg:
      txt += QString("Warning: %1").arg(message);
      break;

    case QtCriticalMsg:
      txt = QString("Critical: %1").arg(message);
      break;

    case QtFatalMsg:
      txt = QString("Fatal: %1").arg(message);
      break;

    case QtInfoMsg:
      txt = message;
      break;
  }

  QTextStream screen_output(stdout);
  if constexpr (kServerShowDebugMessagesOnScreen) {
    screen_output << txt << Qt::endl;
    if (type != QtInfoMsg) {
      screen_output << QString("        At %1 (%2:%3)")
          .arg(context.function)
          .arg(context.file)
          .arg(context.line) << Qt::endl;
    }
  } else {
    if (type == QtInfoMsg) {
      screen_output << txt << Qt::endl;
    }
  }

  static QFile out_file("server.log");
  if (!out_file.isOpen()) {
    out_file.open(QIODevice::WriteOnly | QIODevice::Append);
  }

  static QTextStream text_stream(&out_file);
  text_stream << QTime::currentTime().toString("[hh:mm:ss.zzz] - ")
              << txt << Qt::endl;
}

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  QFile::remove("server.log");
  qInstallMessageHandler(ServerMessageHandler);

  auto server_controller = std::make_unique<ServerController>();
  qInfo().noquote().nospace() << "[SERVER] Server started";

  return QCoreApplication::exec();
}
