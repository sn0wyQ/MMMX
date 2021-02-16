#ifndef SERVER_MODEL_H_
#define SERVER_MODEL_H_

#include <memory>
#include <vector>

#include <QWebSocket>
#include <QWebSocketServer>

class ServerModel {
 public:
  ServerModel() = default;

 private:
  struct PlayerOnServer {
    int id;
    std::unique_ptr<QWebSocket> socket;
  };

  QWebSocketServer web_socket_server_;
  std::vector<PlayerOnServer> players_on_server_;
};

#endif  // SERVER_MODEL_H_
