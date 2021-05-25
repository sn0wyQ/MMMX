#include <memory>

#include <QApplication>
#include <QFontDatabase>
#include <QScreen>

#include "Client/client_controller.h"
#include "GUI/client_view.h"
#include "MessageHandler/message_handler.h"

MessageHandler message_handler(Constants::kClientEnableIgnoreLevel,
                               Constants::kClientMessageIgnoreLevel,
                               "client.log");

void MessageHandlerWrapper(QtMsgType type,
                           const QMessageLogContext& context,
                           const QString& message) {
  message_handler.Handle(type, context, message);
}

int main(int argc, char* argv[]) {
  bool is_remote = true;
  if (argc > 1 && strcmp(argv[1], "-local") == 0) {
    is_remote = false;
  }

  QString server_ip = is_remote ? "188.120.224.70" : "localhost";
  QUrl server_url =
      QUrl(QString("ws://") + server_ip + ":" +
      QString::number(Constants::kServerPort));

  QSurfaceFormat surface_format = QSurfaceFormat::defaultFormat();
  surface_format.setSamples(Constants::kAntiAliasingSamples);
  QSurfaceFormat::setDefaultFormat(surface_format);

  QApplication app(argc, argv);
  QFontDatabase::addApplicationFont(":Res/Fonts/CynthoNext-Bold.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Regular.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Bold.ttf");
  QFile::remove("client.log");
  qInstallMessageHandler(MessageHandlerWrapper);

  auto* client_controller = new ClientController(
      server_url, QApplication::primaryScreen()->refreshRate());
  auto* client_view = new ClientView(client_controller);
  QScreen *screen = QGuiApplication::screens()[0];
  client_view->move(screen->geometry().x(), screen->geometry().y());
  client_view->setFixedSize(screen->geometry().width(),
                            screen->geometry().height());
  client_view->showFullScreen();
  client_view->show();

  return QApplication::exec();
}
