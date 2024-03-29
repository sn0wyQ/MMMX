#include <QApplication>
#include <QFontDatabase>
#include <QScreen>

#include "Client/client_controller.h"
#include "GUI/client_view.h"
#include "MessageHandler/message_handler.h"
#include "Settings/settings.h"

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

  QApplication app(argc, argv);
  QFontDatabase::addApplicationFont(":Res/Fonts/CynthoNext-Bold.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Regular.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Bold.ttf");
  QFile::remove("client.log");
  qInstallMessageHandler(MessageHandlerWrapper);
  QSurfaceFormat surface_format = QSurfaceFormat::defaultFormat();
  surface_format.setSamples(Settings::GetInstance().GetAntialiasingSamples());
  QSurfaceFormat::setDefaultFormat(surface_format);

  auto* client_controller = new ClientController(
      server_url, QApplication::primaryScreen()->refreshRate());
  auto* client_view = new ClientView(client_controller);
  QFile style_file(":stylesheet.qss");
  if (style_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    client_view->setStyleSheet(style_file.readAll());
    style_file.close();
  }

  client_view->show();
  if (Settings::GetInstance().GetValueByKey<bool>("main/fullscreen")) {
    client_view->SetFullScreen();
  }

  return QApplication::exec();
}
