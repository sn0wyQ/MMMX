#include <QCoreApplication>
#include <QDebug>

#include <memory>

#include "Server/server_controller.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  auto server_controller = std::make_unique<ServerController>();
  qInfo().noquote().nospace() << "[SERVER] Server started";
  return QCoreApplication::exec();
}
