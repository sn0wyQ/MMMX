#ifndef SERVER_MODEL_H_
#define SERVER_MODEL_H_

#include <QWebSocket>
#include <QWebSocketServer>

class ServerModel {
 public:
  ServerModel() = default;

 private:
  struct PlayerOnServer {
    int id;
    QWebSocket socket;
  };

  QWebSocketServer web_socket_server_;
};

#endif  // SERVER_MODEL_H_
