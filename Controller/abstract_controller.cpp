#include "abstract_controller.h"

void AbstractController::Tick() {
  Listen();
  while (!game_events_to_handle_.empty()) {
    HandleGameEvent(game_events_to_handle_.front());
    game_events_to_handle_.pop();
  }
  Send();
}

template<typename... Args>
void AbstractController::AddGameEventToHandle(Args... args) {
  game_events_to_handle_.emplace(args...);
}

template<typename... Args>
void AbstractController::AddGameEventToSend(Args... args) {
  game_events_to_send_.emplace(args...);
}

void AbstractController::StartGame() {
  AddGameEventToHandle(GameEventType::kStartGame);
  ticker_.start(Constants::kTimeToTick);
  last_tick_.start();
}

void AbstractController::EndGame() {
  AddGameEventToHandle(GameEventType::kEndGame);
  ticker_.stop();
  last_tick_.invalidate();
}
