#ifndef EVENT_EVENT_TYPE_H_
#define EVENT_EVENT_TYPE_H_

namespace EventTypeWrapper {

Q_NAMESPACE

enum class EventType {
  // MUST always stay in alphabet order
  // Also MUST have comments with arguments list
  // After adding an event, add a function with the same name
  // to the BaseController private functions

  // [0] - <int> client_id,
  // [1] - <int> player_id
  kAddNewPlayer,

  // [0] - <int> client_id
  kClientDisconnected,

  // No arguments
  kEndGame,

  // [0] - <int> player_id
  kPlayerDisconnected,

  // [0] - <int> client_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToClient,

  // [0] - <int> room_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToRoom,

  // [0] - <int> client_id,
  // [1] - <int> player_id
  kSetClientsPlayerId,

  // [0] - <int> client_id,
  // [1] - <int> number_of_players,
  // [2+] - player_info [<int> player_id,
  //                     <float> x,
  //                     <float> y,
  //                     <float> view_angle]
  kSharePlayersInRoomInfo,

  // No arguments
  kStartGame,

  // ------------------- GAME EVENTS -------------------

  // [0] - <int> sender_player_id,
  // [1] - <uint32> mask 0000 means direction
  // up/right/down/left direction true/false
  kSendDirectionInfo,

  // [0] - <int> sender_player_id,
  // [1] - <float> view_angle
  kSendViewAngle,

  // [0] - <int> sender_player_id,
  // [1] - <float> x,
  // [2] - <float> y
  kUpdatePlayerPosition,

  // [0] - <int> sender_PLAYER_id / receiver_CLIENT_id
  kUpdateServerVar,

  SIZE
};

Q_ENUM_NS(EventType)

}  // EventTypeWrapper

using EventType = EventTypeWrapper::EventType;
Q_DECLARE_METATYPE(EventType)

#endif  // EVENT_EVENT_TYPE_H_
