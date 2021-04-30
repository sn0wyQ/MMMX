#include <memory>

#include <QCoreApplication>

#include "MessageHandler/message_handler.h"
#include "Server/server_controller.h"

MessageHandler message_handler(Constants::kServerEnableIgnoreLevel,
                               Constants::kServerMessageIgnoreLevel,
                               "server.log");

void MessageHandlerWrapper(QtMsgType type,
                           const QMessageLogContext& context,
                           const QString& message) {
  message_handler.Handle(type, context, message);
}

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  QFile::remove("server.log");

  qInstallMessageHandler(MessageHandlerWrapper);

  auto server_controller = std::make_unique<ServerController>();
  qInfo().noquote().nospace() << "[SERVER] Server started";

  return QCoreApplication::exec();
}
