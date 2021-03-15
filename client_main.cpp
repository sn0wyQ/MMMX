#include <memory>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTime>

#include "Client/client_controller.h"
#include "Client/client_view.h"

void ClientMessageHandler(QtMsgType type,
                          const QMessageLogContext& context,
                          const QString& message) {
#if CLIENT_SHOW_ONLY_INFO_MESSAGES
  if (type != QtInfoMsg) {
    return;
  }
#endif  // CLIENT_SHOW_ONLY_INFO_MESSAGES

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
#if CLIENT_SHOW_DEBUG_MESSAGES_ON_SCREEN
  screen_output << txt << Qt::endl;
  if (type != QtInfoMsg) {
    screen_output << QString("        At %1 (%2:%3)")
        .arg(context.function)
        .arg(context.file)
        .arg(context.line) << Qt::endl;
  }
#else
  if (type == QtInfoMsg) {
    screen_output << txt << Qt::endl;
  }
#endif  // CLIENT_SHOW_DEBUG_MESSAGES

  static QFile out_file("client.log");
  if (!out_file.isOpen()) {
    out_file.open(QIODevice::WriteOnly | QIODevice::Append);
  }

  static QTextStream text_stream(&out_file);
  text_stream << QTime::currentTime().toString("[hh:mm:ss.zzz] - ")
              << txt << Qt::endl;
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QFile::remove("client.log");
  qInstallMessageHandler(ClientMessageHandler);

  auto client_controller = std::make_shared<ClientController>();
  auto client_view = new ClientView(client_controller);
  client_view->show();

  return QApplication::exec();
}
