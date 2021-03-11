#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <memory>
#include <utility>

#include <QByteArray>
#include <QDebug>
#include <QUrl>
#include <QWebSocket>

#include "Client/abstract_client_view.h"
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

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick() override;

  GameDataModel* GetModel();

  void SetGameState(GameState game_state);
  bool IsInProgress() const;

  void SetView(std::shared_ptr<AbstractClientView> view);

  void ReceiveEvent(const Event& controls_event);

  public Q_SLOTS:
  void OnConnected();
  void OnDisconnected();
  void OnByteArrayReceived(const QByteArray& message);

 private:
  GameState game_state_ = GameState::kNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  GameDataModel model_;
  std::shared_ptr<AbstractClientView> view_;

  void AddNewPlayerEvent(const Event& event) override;
  void EndGameEvent(const Event& event) override;
  void ChangedTestCounterEvent(const Event& event) override;
  void PlayerDisconnectedEvent(const Event& event) override;
  void PressedTestButtonEvent(const Event& event) override;
  void SetClientsPlayerIdEvent(const Event& event) override;
  void SharePlayersInRoomIdsEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
