#ifndef ABSTRACT_CONTROLLER_H_
#define ABSTRACT_CONTROLLER_H_

#include <memory>
#include <queue>

#include <QElapsedTimer>
#include <QTimer>

#include "GameEvent/game_event.h"
#include "Model/game_data_model.h"
#include "constants.h"

class AbstractController {
 public:
  virtual ~AbstractController() = 0;

  // Every Tick() we successively do following:
  // 1) Call Listen()
  // 2) Call HandleGameEvent() for every GameEvent from game_events_to_handle_
  // 3) Call Send()
  void Tick();
  virtual void Listen() = 0;
  virtual void HandleGameEvent(const GameEvent&) = 0;
  virtual void Send() = 0;

  template<typename... Args>
  void AddGameEventToHandle(Args... args);
  // MUST be called ONLY from HandleGameEvent(const GameEvent&)
  template<typename... Args>
  void AddGameEventToSend(Args... args);

  void StartGame();
  void EndGame();

 private:
  QTimer ticker_;
  QElapsedTimer last_tick_;

  std::queue<GameEvent> game_events_to_handle_;
  std::queue<GameEvent> game_events_to_send_;
};

#endif  // ABSTRACT_CONTROLLER_H_
