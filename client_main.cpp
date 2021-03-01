#include <QApplication>

#include "Client/client_controller.h"
#include "Client/client_view.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  auto client_controller = std::make_shared<ClientController>();
  auto client_view = new ClientView(client_controller, nullptr);
  client_view->show();

  return QApplication::exec();
}
