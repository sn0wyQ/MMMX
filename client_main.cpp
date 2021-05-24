#include <memory>

#include <QApplication>
#include <QFontDatabase>
#include <QScreen>

#include "Client/client_controller.h"
#include "Client/client_view.h"
#include "MessageHandler/message_handler.h"

MessageHandler message_handler(Constants::kClientEnableIgnoreLevel,
                               Constants::kClientMessageIgnoreLevel,
                               "client.log");

void MessageHandlerWrapper(QtMsgType type,
                           const QMessageLogContext& context,
                           const QString& message) {
  message_handler.Handle(type, context, message);
}

#include "Event/packed_event.h"

int main(int argc, char* argv[]) {
  bool is_remote = true;
  if (argc > 1 && strcmp(argv[1], "-local") == 0) {
    is_remote = false;
  }

  PackedEvent packed_event;
  packed_event.AddEvent(Event(EventType::kSendNickname, QVariant(5)));
  packed_event.AddEvent(Event(EventType::kPlayerRespawned, QVariant("GAYs")));
  packed_event.AddEvent(Event(EventType::kSendControls, QVariant("SGAY")));
  auto bytes = packed_event.ToByteArray();
  auto new_packed = PackedEvent(bytes);
  for (auto event : new_packed.GetEvents()) {
    qWarning() << event;
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

  auto client_controller = std::make_shared<ClientController>(
      server_url, QApplication::primaryScreen()->refreshRate());
  auto client_view = new ClientView(client_controller);
  client_view->show();

  return QApplication::exec();
}
