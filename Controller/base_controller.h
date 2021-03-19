#ifndef CONTROLLER_BASE_CONTROLLER_H_
#define CONTROLLER_BASE_CONTROLLER_H_

#include <functional>
#include <map>
#include <memory>
#include <queue>

#include <QDebug>
#include <QElapsedTimer>
#include <QString>
#include <QTimer>

#include "Event/event.h"
#include "Model/game_data_model.h"
#include "constants.h"

class BaseController : public QObject {
 public:
  ~BaseController() override = default;

  virtual QString GetControllerName() const = 0;

  // Every Tick() we successively do following:
  // 1) Call OnTick(int time_from_previous_tick)
  // 2) Call HandleEvent() for every Event from events_to_handle_
  // 3) Call SendEvent() for every Event from events_to_send_
  // 4) Call ResetTimeFromPreviousTick()
  void Tick();
  virtual void OnTick(int time_from_previous_tick) = 0;

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

 protected:
  BaseController();

  void SetFunctionForEventType(EventType event_type,
                               const std::function<void(const Event&)>& func);

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;

  QElapsedTimer var_timer_;
  int var_;

  std::queue<Event> events_to_handle_;
  std::queue<Event> events_to_send_;

  std::array<std::function<void(const Event&)>,
      static_cast<uint32_t>(EventType::SIZE)> function_for_event_;

  virtual void AddNewPlayerEvent(const Event& event) {}
  virtual void ClientDisconnectedEvent(const Event& event) {}
  virtual void EndGameEvent(const Event& event) {}
  virtual void PlayerDisconnectedEvent(const Event& event) {}
  virtual void SetClientsPlayerIdEvent(const Event& event) {}
  virtual void CreateAllPlayersDataEvent(const Event& event) {}
  virtual void StartGameEvent(const Event& event) {}
  virtual void SendEventToClientEvent(const Event& event) {}
  virtual void SendEventToRoomEvent(const Event& event) {}
  virtual void UpdateVarsEvent(const Event& event) {}

  // ------------------- GAME EVENTS -------------------

  virtual void SendControlsEvent(const Event& event) {}
  virtual void UpdatePlayerDataEvent(const Event& event) {}
};

#endif  // CONTROLLER_BASE_CONTROLLER_H_
