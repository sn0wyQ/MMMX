#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <QByteArray>
#include <QDebug>
#include <QUrl>
#include <QWebSocket>

#include "Controller/base_controller.h"
#include "Model/game_data_model.h"

enum class GameState {
  kFinished,
  kInProgress,
  kNotStarted
};

class ClientController : public BaseController {
  Q_OBJECT

 public:
  explicit ClientController(const QUrl& url = Constants::kServerUrl);
  ~ClientController() override = default;

  void HandleEvent(const Event& event) override;
  void Send() override;

  GameDataModel* GetModel();

  void SetGameState(GameState game_state);
  bool IsInProgress() const;

  Q_SIGNALS:
  void UpdateView();

 public Q_SLOTS:
  void OnConnected();
  void OnDisconnected();
  void OnControlsEventReceived(const Event& controls_event);
  void OnByteArrayReceived(const QByteArray& message);

 private:
  GameState game_state_ = GameState::kNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  GameDataModel model_;
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
