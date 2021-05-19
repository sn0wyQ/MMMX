#ifndef CLIENT_CLIENT_CONTROLLER_H_
#define CLIENT_CLIENT_CONTROLLER_H_

#include <algorithm>
#include <ctime>
#include <memory>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

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

enum class GameState {
  kGameFinished,
  kGameInProgress,
  kGameNotStarted
};

class ClientController : public BaseController {
  Q_OBJECT

 public:
  ClientController(const QUrl& url,
                   int fps_max = Constants::kDefaultFpsMax);
  ~ClientController() override = default;

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick(int delta_time) override;

  void OnTickGameFinished(int) {}
  void OnTickGameInProgress(int delta_time);
  void OnTickGameNotStarted(int delta_time);
  void SendPlayerDataToServer();

  std::shared_ptr<ClientGameModel> GetModel();
  int GetServerVar() const;
  int GetRoomVar() const;
  int GetClientVar() const;
  int GetPing() const;

  bool IsGameInProgress() const;

  int64_t GetHoldingRespawnButtonMsecs() const;
  int64_t GetSecsToNextPossibleRevive() const;
  bool GetIsHoldingRespawnButton() const;

  void SetView(std::shared_ptr<AbstractClientView> view);
  void UpdateView();

  void UpdateAnimations(int delta_time);
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
  void ControlsHolding();

 private:
  void EndGameEvent(const Event& event) override;
  void PlayerConnectedEvent(const Event& event) override;
  void PlayerDisconnectedEvent(const Event& event) override;
  void SetPlayerIdToClient(const Event& event) override;
  void SetTimeDifferenceEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
  void UpdateVarsEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void AddLocalPlayerGameObjectEvent(const Event& event) override;
  void DeleteGameObjectEvent(const Event& event) override;
  void IncreaseLocalPlayerExperienceEvent(const Event& event) override;
  void ShootFailedEvent(const Event& event) override;
  void LocalPlayerDiedEvent(const Event& event) override;
  void ReviveLocalPlayerEvent(const Event& event) override;
  void SendGameInfoToInterpolateEvent(const Event& event) override;
  void PlayerKilledNotificationEvent(const Event& event) override;
  void PlayerRespawnedEvent(const Event& event) override;
  void UpdateGameObjectDataEvent(const Event& event) override;
  void UpdatePlayersStatsEvent(const Event& event) override;
  void UpdateLocalPlayerHealthPointsEvent(const Event& event) override;

  QString GetEntityName(GameObjectId game_object_id) const;

  GameState game_state_ = GameState::kGameNotStarted;
  QUrl url_;
  QWebSocket web_socket_;
  std::shared_ptr<ClientGameModel> model_;
  std::shared_ptr<AbstractClientView> view_;
  std::shared_ptr<KeyController> key_controller_;
  int server_var_{0};
  int room_var_{0};
  int client_var_{0};
  int ping_{0};
  int fps_max_{Constants::kDefaultFpsMax};
  QTimer server_var_timer_;
  QTimer view_update_timer_;
  std::shared_ptr<Converter> converter_;
  bool is_time_difference_set_{false};
  int64_t time_difference_{0};
  int64_t last_view_update_time_{-1};
  QPointF last_mouse_position_;
  QTimer controls_check_timer_;
  bool are_controls_blocked_{false};

  int64_t last_died_{0};
  int64_t last_requested_respawn_time_{0};
  int64_t respawn_holding_current_{0};

  std::queue<std::pair<GameObjectId, int64_t>> time_to_delete_;
};

#endif  // CLIENT_CLIENT_CONTROLLER_H_
