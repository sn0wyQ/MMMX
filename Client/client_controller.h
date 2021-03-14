#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <memory>
#include <utility>

#include <QByteArray>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QWebSocket>

#include "Client/abstract_client_view.h"
#include "Controller/base_controller.h"
#include "Model/game_data_model.h"


enum class Direction {
  kUp,
  kRight,
  kDown,
  kLeft,
  SIZE
};

enum class GameState {
  kFinished,
  kGameInProgress,
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
  int GetServerVar() const;
  int GetPing() const;

  bool IsGameInProgress() const;

  void SetView(std::shared_ptr<AbstractClientView> view);

  // -------------------- CONTROLS --------------------
  void ApplyDirection();
  void ResetDirection();

  void KeyPressEvent(QKeyEvent* key_event);
  void KeyReleaseEvent(QKeyEvent* key_event);
  void MouseMoveEvent(QMouseEvent* mouse_event);

  public Q_SLOTS:
  void OnConnected();
  void OnDisconnected();
  void OnByteArrayReceived(const QByteArray& message);
  void UpdateServerVar();
  void UpdatePing(int elapsed_time);

 private:
  GameState game_state_ = GameState::kNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  GameDataModel model_;
  std::shared_ptr<AbstractClientView> view_;
  int server_var_{0};
  int ping_{0};
  QTimer timer_for_ping_;
  QElapsedTimer timer_elapsed_ping_;

  void AddNewPlayerEvent(const Event& event) override;
  void EndGameEvent(const Event& event) override;
  void PlayerDisconnectedEvent(const Event& event) override;
  void SetClientsPlayerIdEvent(const Event& event) override;
  void SharePlayersInRoomInfoEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
  void UpdateServerVarEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void SendDirectionInfoEvent(const Event& event) override;
  void UpdatedPlayerPositionEvent(const Event& event) override;

  // TODO(Everyone): Rework
  std::unordered_map<uint32_t, Direction> key_to_direction_{
      {Qt::Key_W, Direction::kUp},
      {Qt::Key_D, Direction::kRight},
      {Qt::Key_S, Direction::kDown},
      {Qt::Key_A, Direction::kLeft}
  };
  std::unordered_map<Direction, bool> is_direction_by_keys_{false};
  std::unordered_map<Direction, bool> is_direction_applied_{false};
  QTimer* timer_for_keys_;
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
