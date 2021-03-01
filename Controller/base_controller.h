#ifndef CONTROLLER_BASE_CONTROLLER_H_
#define CONTROLLER_BASE_CONTROLLER_H_

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
  BaseController();
  ~BaseController() override = default;

  // Every Tick() we successively do following:
  // 1) Call HandleEvent() for every Event from events_to_handle_
  // 2) Call Send()
  void Tick();
  virtual void HandleEvent(const Event&) = 0;
  virtual void Send() = 0;

  template<typename... Args>
  void AddEventToHandle(Args... args) {
    events_to_handle_.emplace(args...);
  }
  // MUST be called ONLY from HandleEvent(...)
  template<typename... Args>
  void AddEventToSend(Args... args) {
    events_to_send_.emplace(args...);
  }

  bool HasEventsToSend();
  Event GetNextEventToSend();

  void StartTicking();
  void StopTicking();

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;

  std::queue<Event> events_to_handle_;
  std::queue<Event> events_to_send_;
};

#endif  // CONTROLLER_BASE_CONTROLLER_H_
