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

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QFontDatabase::addApplicationFont(":Res/Fonts/CynthoNext-Bold.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Regular.ttf");
  QFontDatabase::addApplicationFont(":Res/Fonts/RobotoMono-Bold.ttf");
  QFile::remove("client.log");
  qInstallMessageHandler(MessageHandlerWrapper);

  auto client_controller = std::make_shared<ClientController>(
      QApplication::primaryScreen()->refreshRate());
  auto client_view = new ClientView(client_controller);
  client_view->show();

  return QApplication::exec();
}
