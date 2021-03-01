#include <QCoreApplication>
#include <QDebug>

#include <memory>

#include "Server/server_controller.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  std::unique_ptr<ServerController> server_controller =
      std::make_unique<ServerController>();
  qInfo() << "[SERVER] Server started";
  return QCoreApplication::exec();
}
