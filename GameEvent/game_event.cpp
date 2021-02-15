#include "game_event.h"

#include <utility>

GameEvent::GameEvent(const GameEvent& game_event) {
  type_ = game_event.type_;
  args_ = game_event.args_;
}

GameEvent::GameEvent(GameEvent&& game_event) noexcept {
  type_ = game_event.type_;
  args_.swap(game_event.args_);
}

GameEvent::GameEvent(GameEventType game_event_type)
  : type_(game_event_type) {}

template<typename... Args>
GameEvent::GameEvent(GameEventType game_event_type, Args... args)
  : type_(game_event_type), args_({args...}) {}

InvalidGameEventException::InvalidGameEventException(GameEvent game_event)
  : game_event_(std::move(game_event)) {}

void InvalidGameEventException::raise() const {
  throw *this;
}

InvalidGameEventException* InvalidGameEventException::clone() const {
  return new InvalidGameEventException(*this);
}

GameEvent InvalidGameEventException::game_event() const {
  return game_event_;
}
