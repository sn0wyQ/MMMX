#ifndef EVENT_EVENT_TYPE_H_
#define EVENT_EVENT_TYPE_H_

namespace EventTypeWrapper {

Q_NAMESPACE

enum class EventType {
  // MUST always stay in alphabet order
  // Also MUST have comments with arguments list
  // After adding an event, add a function with the same name
  // to the BaseController private functions


  // [0] - <ClientId> client_id
  kClientDisconnected,

  // [0] - <Bullet> bullet
  kCreateBullet,

  // [0] - <Bullet> bullet
  kDeleteBullet,

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

  // No arguments
  kSendGetVars,

  // [0] - <GameObjectId> player_id
  kSetPlayerIdToClient,

  // No arguments
  kStartGame,

  // [0] - <int> server_var,
  // [1] - <int> room_var
  kUpdateVars,

  // ------------------- GAME EVENTS -------------------

  // [0] - <GameObjectId> game_object_id that left receiver player FOV
  kGameObjectLeftFov,

  // [0] - <GameObjectId> sender_player_id,
  // [1] - <float> x,
  // [2] - <float> y,
  // [3] - <QVector2D> velocity,
  // [4] - <float> rotation
  kSendControls,

  // [0] - <GameObjectId> game_object_id
  // [1] - <int> game_object_type [game_object.h]
  // [2+] - game_object_args : relative constructor args
  kUpdateGameObjectData,

  SIZE
};

Q_ENUM_NS(EventType)

}  // namespace EventTypeWrapper

using EventType = EventTypeWrapper::EventType;
Q_DECLARE_METATYPE(EventType)

#endif  // EVENT_EVENT_TYPE_H_
