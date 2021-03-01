#include "event.h"

Event::Event(const Event& event) {
  type_ = event.type_;
  args_ = event.args_;
}

Event::Event(Event&& event) noexcept {
  type_ = event.type_;
  args_.swap(event.args_);
}

Event::Event(const QByteArray& message) {
  QDataStream data_stream(message);

  int type_in_int;
  data_stream >> type_in_int;
  type_ = static_cast<EventType>(type_in_int);

  while (!data_stream.atEnd()) {
    int next_int;
    data_stream >> next_int;
    args_.push_back(next_int);
  }
}

EventType Event::GetType() const {
  return type_;
}

int Event::GetArg(int index) const {
  return args_.at(index);
}

std::vector<int> Event::GetArgs() const {
  return args_;
}

std::vector<int> Event::GetArgsSubVector(int first_index) const {
  return std::vector<int>(args_.begin() + first_index,
                          args_.end());
}

std::vector<int>
    Event::GetArgsSubVector(int first_index, int last_index) const {
  return std::vector<int>(args_.begin() + first_index,
                          args_.begin() + last_index);
}

QByteArray Event::ToByteArray() const {
  QByteArray result;
  QDataStream data_stream(&result, QIODevice::WriteOnly);
  data_stream << static_cast<int>(type_);
  for (auto arg : args_) {
    data_stream << arg;
  }
  return result;
}

QDebug operator<<(QDebug debug, const Event& event) {
  const bool oldSetting = debug.autoInsertSpaces();
  debug.nospace() << "Event(Type: " << static_cast<int>(event.type_);
  for (auto arg : event.args_) {
    debug.nospace() << ", " << arg;
  }
  debug.nospace() << ')';
  debug.setAutoInsertSpaces(oldSetting);
  return debug.maybeSpace();
}

InvalidGameEventException::InvalidGameEventException(Event game_event)
  : game_event_(std::move(game_event)) {}

void InvalidGameEventException::raise() const {
  throw *this;
}

InvalidGameEventException* InvalidGameEventException::clone() const {
  return new InvalidGameEventException(*this);
}

Event InvalidGameEventException::game_event() const {
  return game_event_;
}
