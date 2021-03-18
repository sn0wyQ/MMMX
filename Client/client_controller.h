#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include <QByteArray>
#include <QDebug>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QWebSocket>

#include "Client/abstract_client_view.h"
#include "Controller/base_controller.h"
#include "Converter/converter.h"
#include "Math/math.h"
#include "Model/game_data_model.h"

// TODO(Everyone): make class Hotkeys (with possibility to rebind keys)
// instead of enum Controls

#ifdef WIN32
enum class Controls {
  kKeyW = 17,
  kKeyA = 30,
  kKeyS = 31,
  kKeyD = 32
};
#else
enum class Controls {
  kKeyW = 25,
  kKeyA = 38,
  kKeyS = 39,
  kKeyD = 40
};
#endif

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
  void OnTick(int time_from_previous_tick) override;

  std::shared_ptr<GameDataModel> GetModel();
  int GetServerVar() const;
  int GetPing() const;

  bool IsGameInProgress() const;

  void SetView(std::shared_ptr<AbstractClientView> view);

  // -------------------- CONTROLS --------------------
  void ApplyDirection();
  void ResetDirection();

  void FocusOutEvent(QFocusEvent* focus_event);
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
  void AddNewPlayerEvent(const Event& event) override;
  void EndGameEvent(const Event& event) override;
  void PlayerDisconnectedEvent(const Event& event) override;
  void SetClientsPlayerIdEvent(const Event& event) override;
  void CreateAllPlayersDataEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
  void UpdateServerVarEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void SendControlsEvent(const Event& event) override;
  void UpdatePlayerDataEvent(const Event& event) override;

  GameState game_state_ = GameState::kNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  std::shared_ptr<GameDataModel> model_;
  std::shared_ptr<AbstractClientView> view_;
  int server_var_{0};
  int ping_{0};
  QTimer timer_for_server_var_;
  QElapsedTimer timer_elapsed_server_var_;
  std::shared_ptr<Converter> converter_;

  // TODO(Everyone): Rework
  std::unordered_map<Controls, Direction> key_to_direction_{
      {Controls::kKeyW, Direction::kUp},
      {Controls::kKeyD, Direction::kRight},
      {Controls::kKeyS, Direction::kDown},
      {Controls::kKeyA, Direction::kLeft}
  };
  std::unordered_map<Direction, bool> is_direction_by_keys_{false};
  std::unordered_map<Direction, bool> is_direction_applied_{false};
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
