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

  // Every Tick() we successively do following:
  // 1) Call HandleEvent() for every Event from events_to_handle_
  // 2) Call Send()
  void Tick();
  void HandleEvent(const Event& event);
  virtual void SendEvent(const Event& event) = 0;

  void AddEventToHandle(const Event& event);
  // MUST be called ONLY from HandleEvent(...)
  void AddEventToSend(const Event& event);

  bool HasEventsToSend() const;
  bool HasEventsToHandle() const;

  void StartTicking();
  void StopTicking();

 protected:
  BaseController();

  void SetFunctionForEvent(const Event& event,
                           std::function<void(const Event&)> func);

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;
  std::queue<Event> events_to_handle_;
  std::queue<Event> events_to_send_;

  std::map<EventType, std::function<void(const Event&)>> function_for_event_{};

  virtual void AddNewPlayerEvent(const Event& event) {}
  virtual void ClientDisconnectedEvent(const Event& event) {}
  virtual void EndGameEvent(const Event& event) {}
  virtual void ChangedTestCounterEvent(const Event& event) {}
  virtual void PlayerDisconnectedEvent(const Event& event) {}
  virtual void PressedTestButtonEvent(const Event& event) {}
  virtual void SetClientsPlayerIdEvent(const Event& event) {}
  virtual void SharePlayersInRoomIdsEvent(const Event& event) {}
  virtual void StartGameEvent(const Event& event) {}
};

#endif  // CONTROLLER_BASE_CONTROLLER_H_
