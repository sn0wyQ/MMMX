#ifndef CONTROLLER_BASE_CONTROLLER_H_
#define CONTROLLER_BASE_CONTROLLER_H_

#include <functional>
#include <map>
#include <memory>
#include <queue>

#include <QDebug>
#include <QElapsedTimer>
#include <QTimer>

#include "Event/event.h"
#include "Model/game_data_model.h"
#include "constants.h"

class BaseController : public QObject {
 public:
  ~BaseController() override = default;

  virtual QString GetControllerName() const = 0;
  // Every Tick() we successively do following:
  // 1) Call HandleEvent() for every Event from events_to_handle_
  // 2) Call SendEvent() for every Event from events_to_send_
  // 3) Call OnTick()
  void Tick();
  virtual void OnTick() = 0;
  void HandleEvent(const Event& event);
  virtual void SendEvent(const Event& event) = 0;
  void LogEvent(const Event& event) const;

  void AddEventToHandle(const Event& event);
  // MUST be called ONLY from HandleEvent(...)
  void AddEventToSend(const Event& event);

  bool HasEventsToSend() const;
  bool HasEventsToHandle() const;

  void StartTicking();

 protected:
  BaseController();

  void SetFunctionForEventType(EventType event_type,
                               const std::function<void(const Event&)>& func);

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;
  std::queue<Event> events_to_handle_;
  std::queue<Event> events_to_send_;

  std::array<std::function<void(const Event&)>,
      static_cast<uint32_t>(EventType::SIZE)> function_for_event_;

  virtual void AddNewPlayerEvent(const Event& event) {}
  virtual void ClientDisconnectedEvent(const Event& event) {}
  virtual void EndGameEvent(const Event& event) {}
  virtual void PlayerDisconnectedEvent(const Event& event) {}
  virtual void SetClientsPlayerIdEvent(const Event& event) {}
  virtual void SharePlayersInRoomInfoEvent(const Event& event) {}
  virtual void StartGameEvent(const Event& event) {}
  virtual void SendEventToClientEvent(const Event& event) {}
  virtual void SendEventToRoomEvent(const Event& event) {}
  virtual void UpdateServerVarEvent(const Event& event) {}

  // ------------------- GAME EVENTS -------------------

  virtual void SendDirectionInfoEvent(const Event& event) {}
  virtual void UpdatedPlayerPositionEvent(const Event& event) {}
};

#endif  // CONTROLLER_BASE_CONTROLLER_H_
