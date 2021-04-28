#ifndef CONTROLLER_BASE_CONTROLLER_H_
#define CONTROLLER_BASE_CONTROLLER_H_

#include <functional>
#include <map>
#include <memory>
#include <queue>

#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QString>
#include <QTimer>

#include "Event/event.h"
#include "Model/game_model.h"
#include "constants.h"

class BaseController : public QObject {
 public:
  ~BaseController() override = default;

  virtual QString GetControllerName() const = 0;

  // Every Tick() we successively do following:
  // 1) Call OnTick(int delta_time)
  // 2) Call HandleEvent() for every Event from events_to_handle_
  // 3) Call SendEvent() for every Event from events_to_send_
  // 4) Call ResetTimeFromPreviousTick()
  void Tick();
  virtual void OnTick(int delta_time) = 0;

  void HandleEvent(const Event& event);
  virtual void SendEvent(const Event& event) = 0;
  void LogEvent(const Event& event) const;

  void AddEventToHandle(const Event& event);
  // MUST be called ONLY from HandleEvent(...)
  void AddEventToSend(const Event& event);

  bool HasEventsToSend() const;
  bool HasEventsToHandle() const;

  void StartTicking();

  int GetVar() const;
  virtual int64_t GetCurrentServerTime() const;

 protected:
  BaseController();

  void SetFunctionForEventType(
      EventType event_type,
      const std::function<void(const Event&)>& func);

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;

  int var_;

  std::queue<Event> events_to_handle_;
  std::queue<Event> events_to_send_;

  std::array<std::function<void(const Event&)>,
      static_cast<uint32_t>(EventType::SIZE)> function_for_event_;

  virtual void ClientDisconnectedEvent(const Event& event) {}
  virtual void EndGameEvent(const Event& event) {}
  virtual void PlayerDisconnectedEvent(const Event& event) {}
  virtual void SendEventToClientEvent(const Event& event) {}
  virtual void SendEventToRoomEvent(const Event& event) {}
  virtual void SendNicknameEvent(const Event& event) {}
  virtual void SetPlayerIdToClient(const Event& event) {}
  virtual void SetTimeDifferenceEvent(const Event& event) {}
  virtual void StartGameEvent(const Event& event) {}
  virtual void UpdateVarsEvent(const Event& event) {}

  // ------------------- GAME EVENTS -------------------

  virtual void AddLocalPlayerGameObjectEvent(const Event& event) {}
  virtual void GameObjectLeftFovEvent(const Event& event) {}
  virtual void SendControlsEvent(const Event& event) {}
  virtual void SendGameInfoToInterpolateEvent(const Event& event) {}
  virtual void SendPlayerShootingEvent(const Event& event) {}
  virtual void UpdateGameObjectDataEvent(const Event& event) {}
  virtual void UpdatePlayersStatsEvent(const Event& event) {}
  virtual void UpdateLocalPlayerHealthPointsEvent(const Event& event) {}
  virtual void LocalPlayerDiedEvent(const Event& event) {}
  virtual void IncreaseLocalPlayerExperienceEvent(const Event& event) {}
};

#endif  // CONTROLLER_BASE_CONTROLLER_H_
