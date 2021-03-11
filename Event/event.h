#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

#include <utility>
#include <vector>

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QObject>
#include <QMetaEnum>

class Event : public QObject {
  Q_OBJECT
  Q_PROPERTY(EventType event_type MEMBER type_)
  Q_PROPERTY(std::vector<int> event_args MEMBER args_)

 public:
  enum class EventType {
    // MUST always stay in alphabet order
    // Also MUST have comments with arguments list
    // After adding an event, add a function with the same name
    // to the BaseController private functions

    // [0] - client_id
    // [1] - player_id
    kAddNewPlayer,

    // [0] - client_id
    kClientDisconnected,

    // No arguments
    kEndGame,

    // [0] - counter_owner_player_id,
    // [1] - delta,
    // [2] - result
    kChangedTestCounter,

    // [0] - player_id
    kPlayerDisconnected,

    // [0] - sender_player_id,
    // [1] - random_test_int
    kPressedTestButton,

    // [0] - client_id,
    // [1] - event_type,
    // [2+] - event_args
    kSendEventToClient,

    // [0] - room_id,
    // [1] - event_type,
    // [2+] - event_args
    kSendEventToRoom,

    // [0] - client_id
    // [1] - player_id
    kSetClientsPlayerId,

    // [0] - client_id,
    // [1] - number_of_players,
    // [2+] - player_ids
    kSharePlayersInRoomIds,

    // No arguments
    kStartGame,

    SIZE
  };
  Q_ENUM(EventType)

  Event(const Event& event);
  explicit Event(const QByteArray& message);
  template<typename... Args>
  explicit Event(EventType event_type, Args... args)
    : type_(event_type), args_({args...}) {}

  EventType GetType() const;
  int GetArg(int index) const;
  std::vector<int> GetArgs() const;
  // Gets sub-vector of |args_| elements in range [first_index, max_index]
  std::vector<int> GetArgsSubVector(int first_index) const;
  // Gets sub-vector of |args_| elements in range [first_index, last_index)
  std::vector<int> GetArgsSubVector(int first_index, int last_index) const;

  QByteArray ToByteArray() const;


  friend QDebug operator<<(QDebug debug, const Event& event);

 private:
  EventType type_;
  std::vector<int> args_;
};

using EventType = Event::EventType;

#endif  // EVENT_EVENT_H_
