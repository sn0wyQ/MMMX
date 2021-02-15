#ifndef GAME_EVENT_H_
#define GAME_EVENT_H_

#include <vector>

#include <QException>
#include <QObject>

enum class GameEventType {
  // MUST always stay in alphabet order

  kEndGame,
  kProcessEvent,  // [1] - game_event_id_, [2+] - game_event_params_
  kSendEventToClient,  // [1] - game_event_id_, [2+] - game_event_params_
  kStartGame,
};

class GameEvent : public QObject {
  Q_OBJECT
  Q_ENUM(GameEventType)
  Q_PROPERTY(GameEventType game_event_type MEMBER type_)
  Q_PROPERTY(std::vector<int> arguments MEMBER args_)

 public:
  GameEvent(const GameEvent& game_event);
  GameEvent(GameEvent&& game_event) noexcept;
  explicit GameEvent(GameEventType game_event_type);
  template<typename... Args>
  GameEvent(GameEventType game_event_type, Args... args);

 private:
  GameEventType type_;
  std::vector<int> args_;
};

class InvalidGameEventException : public QException {
 public:
  explicit InvalidGameEventException(GameEvent game_event);
  ~InvalidGameEventException() override = default;

  void raise() const override;
  InvalidGameEventException* clone() const override;

  GameEvent game_event() const;

 private:
  GameEvent game_event_;
};

#endif // GAME_EVENT_H_
