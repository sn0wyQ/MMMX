#ifndef EVENT_EVENT_TYPE_H_
#define EVENT_EVENT_TYPE_H_

#include <QObject>

class EventTypeWrapper : public QObject {
  Q_OBJECT

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

    // [0] - player_id
    kPlayerDisconnected,

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
    // [2+] - player_info [player_id, x, y]
    kSharePlayersInRoomInfo,

    // No arguments
    kStartGame,

    // ------------------- GAME EVENTS -------------------

    // [0] - sender_player_id
    // [1] - mask 0000 means direction
    // up/right/down/left direction true/false
    kSendDirectionInfo,

    // [0] - sender_player_id
    // [1] - x
    // [2] - y
    kUpdatedPlayerPosition,

    SIZE
  };

  Q_ENUM(EventType)
};

using EventType = EventTypeWrapper::EventType;

#endif  // EVENT_EVENT_TYPE_H_
