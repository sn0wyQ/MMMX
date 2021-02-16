#include <QCoreApplication>
#include <QDebug>

#include "Controller/abstract_controller.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  qDebug() << "Server started";
  return QCoreApplication::exec();
}
