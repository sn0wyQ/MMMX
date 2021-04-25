#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

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
#include "GameObject/RigidBody/object_collision.h"
#include "Math/math.h"
#include "Model/client_game_model.h"
#include "Interpolator/interpolator.h"

// TODO(Everyone): make class Hotkeys instead of enum Controls
// (with possibility to rebind keys)

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
  kGameFinished,
  kGameInProgress,
  kGameNotStarted
};

class ClientController : public BaseController {
  Q_OBJECT

 public:
  explicit ClientController(const QUrl& url = Constants::kServerUrl);
  ~ClientController() override = default;

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick(int delta_time) override;

  void OnTickGameFinished(int) {}
  void OnTickGameInProgress(int delta_time);
  void OnTickGameNotStarted(int delta_time);

  std::shared_ptr<ClientGameModel> GetModel();
  int GetServerVar() const;
  int GetRoomVar() const;
  int GetClientVar() const;
  int GetPing() const;

  bool IsGameInProgress() const;

  void SetView(std::shared_ptr<AbstractClientView> view);

  void UpdateLocalPlayer(int delta_time);
  void UpdateLocalBullets(int delta_time);
  void UpdateInterpolationInfo();
  int64_t GetCurrentServerTime() const override;

  // -------------------- CONTROLS --------------------

  QVector2D GetKeyForce() const;

  void FocusOutEvent(QFocusEvent*);
  void KeyPressEvent(QKeyEvent* key_event);
  void KeyReleaseEvent(QKeyEvent* key_event);
  void MouseMoveEvent(QMouseEvent* mouse_event);
  void MousePressEvent(QMouseEvent* mouse_event);
  void MouseReleaseEvent(QMouseEvent* mouse_event);

  // --------------------------------------------------

  public Q_SLOTS:
  void OnConnected();
  void OnDisconnected();
  void OnByteArrayReceived(const QByteArray& message);
  void UpdateVarsAndPing();
  void SetPing(int elapsed_time);
  void MouseHolding();

 private:
  void EndGameEvent(const Event& event) override;
  void PlayerDisconnectedEvent(const Event& event) override;
  void SetPlayerIdToClient(const Event& event) override;
  void SetTimeDifferenceEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
  void UpdateVarsEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void AddLocalPlayerGameObjectEvent(const Event& event) override;
  void GameObjectLeftFovEvent(const Event& event) override;
  void SendGameInfoToInterpolateEvent(const Event& event) override;
  void UpdateGameObjectDataEvent(const Event& event) override;
  void UpdatePlayersStatsEvent(const Event& event) override;
  void UpdateLocalPlayerHealthPointsEvent(const Event& event) override;
  void LocalPlayerDiedEvent(const Event& event) override;
  void IncreaseLocalPlayerExperienceEvent(const Event& event) override;

  GameState game_state_ = GameState::kGameNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  std::shared_ptr<ClientGameModel> model_;
  std::shared_ptr<AbstractClientView> view_;
  int server_var_{0};
  int room_var_{0};
  int client_var_{0};
  int ping_{0};
  QTimer timer_for_server_var_;
  std::shared_ptr<Converter> converter_;
  bool is_time_difference_set_{false};
  int64_t time_difference_{0};
  std::unordered_map<Controls, Direction> key_to_direction_{
      {Controls::kKeyW, Direction::kUp},
      {Controls::kKeyD, Direction::kRight},
      {Controls::kKeyS, Direction::kDown},
      {Controls::kKeyA, Direction::kLeft}
  };
  std::unordered_map<Direction, bool> is_direction_by_keys_{
      {Direction::kUp, false},
      {Direction::kRight, false},
      {Direction::kDown, false},
      {Direction::kLeft, false}
  };
  QTimer timer_for_mouse_;
  bool mouse_is_holding_{false};
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
