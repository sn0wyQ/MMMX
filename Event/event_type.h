#ifndef EVENT_EVENT_TYPE_H_
#define EVENT_EVENT_TYPE_H_

namespace EventTypeWrapper {

Q_NAMESPACE

enum class EventType {
  // MUST always stay in alphabet order
  // Also MUST have comments with arguments list
  // After adding an event, add a function with the same name
  // to the BaseController private functions

  // [0] - <ClientId> client_id,
  // [1] - <GameObjectId> player_id
  kAddNewPlayer,

  // [0] - <ClientId> client_id
  kClientDisconnected,

  // [0] - <ClientId> client_id,
  // [1] - <int> number_of_players,
  // [2+] - player_info [<GameObjectId> player_id,
  //                     <float> x,
  //                     <float> y,
  //                     <float> view_angle]
  kCreateAllPlayersData,

  // No arguments
  kEndGame,

  // [0] - <GameObjectId> player_id
  kPlayerDisconnected,

  // [0] - <ClientId> client_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToClient,

  // [0] - <RoomId> room_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToRoom,

  // [0] - <ClientId> client_id,
  // [1] - <GameObjectId> player_id
  kSetClientsPlayerId,

  // No arguments
  kStartGame,

  // [0] - <int> sender_PLAYER_id / receiver_CLIENT_id
  kUpdateServerVar,

  // ------------------- GAME EVENTS -------------------

  // [0] - <GameObjectId> sender_player_id,
  // [1] - <float> x,
  // [2] - <float> y,
  // [3] - <QVector2D> velocity,
  // [4] - <float> view_angle
  kSendControls,

  // [0] - <GameObjectId> sender_player_id,
  // [1] - <float> x,
  // [2] - <float> y,
  // [3] - <QVector2D> velocity,
  // [4] - <float> view_angle
  kUpdatePlayerData,

  // [0] - <ClientId> receiver_client_id,
  // [1] - <uint32_t> new player's FOV
  kUpdatePlayersFOV,

  // [0] - <GameObjectId> sender_player_id
  kPlayerLeftFOV,

  SIZE
};

Q_ENUM_NS(EventType)

}  // namespace EventTypeWrapper

using EventType = EventTypeWrapper::EventType;
Q_DECLARE_METATYPE(EventType)

#endif  // EVENT_EVENT_TYPE_H_
