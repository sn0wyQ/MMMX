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

  // No arguments
  kEndGame,

  // [0] - <ClientId> client_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToClient,

  // [0] - <RoomId> room_id,
  // [1] - <int> event_type,
  // [2+] - event_args
  kSendEventToRoom,

  // [0] - <GameObjectId> player_id,
  // [1] - <QString> new_nickname
  kSendNickname,

  // No arguments
  kSendGetVars,

  // [0] - <GameObjectId> player_id
  kSetPlayerIdToClient,

  // No arguments
  kStartGame,

  // [0] - <int> server_var,
  // [1] - <int> room_var
  kUpdateVars,

  // [0] - <int64> client_sent_time
  // [1] - null / <int64> server_received_time
  // Клиент отправляет в клиентское время X пакет
  // Сервер отвечает тем же запросом, только добавляя серверное время получения
  // Пусть получили запрос на клиенте в клиентское время client_received_time
  // Тогда latency = (client_received_time - client_sent_time) / 2
  // time_difference = server_received_time - client_sent_time - latency
  // Тогда теперь чтобы получить текущее серверное время:
  // server_time = cur_time + time_difference
  // ВНИМАНИЕ: Так как в этих замерах нам важна каждая миллисекунда
  // и мы не хотим зависеть от тикрейта, нам нужно сразу вне тика
  // отправлять и получать этот ивент
  kSetTimeDifference,

  // ------------------- GAME EVENTS -------------------

  // [0] - <GameObjectId> game_object_id
  // [1] - <GameObjectType> game_object_type
  // [2] - <int64> sent_time (on server)
  // [3] - <EventType> event_type
  // [4+] - event_args
  // Send game events to client through this event
  // it will interpolate game object
  kSendGameInfoToInterpolate,

  // Game events to client which interpolate :
  // [[[[[[[[[

  // [0] - <GameObjectId> game_object_id
  // [1+] - game_object_args : relative SetParams
  kUpdateGameObjectData,

  // [0] - <GameObjectId> game_object_id that should be deleted
  // [1] - <int64_t> sent_time
  kDeleteGameObject,

  // ]]]]]]]]]

  // [0] - <GameObjectId> player_id
  // [1+] - player_stats_args
  kUpdatePlayersStats,

  // [0] - <int64_t> timestamp
  kShootFailed,

  // [0] - <int64> timestamp
  // [1] - <GameObjectId> sender_player_id,
  // [2] - <float> x,
  // [3] - <float> y,
  // [4] - <QVector2D> velocity,
  // [5] - <float> rotation
  kSendControls,

  // [0] - <int64> timestamp
  // [1] - <GameObjectId> sender_player_id
  kSendPlayerReloading,

  // [0] - <int64> timestamp
  // [1] - <GameObjectId> sender_player_id
  // [2] - QList<QVariant> random_shifts
  kSendPlayerShooting,

  // [0] - <GameObjectId> sender_player_id
  // LevelingSlots::SIZE times int
  kSendLevelingPoints,

  // [0] - <GameObjectId> player_id
  // [1] - player_args
  kAddLocalPlayerGameObject,

  // [0] - <GameObjectId> player_id
  kPlayerDisconnected,

  // [0] - <GameObjectId> victim player_id
  // [1] - <GameObjectId> killer entity_id
  // [2] - <int> used weapon (WeaponType)
  kPlayerKilledNotification,

  // [0] - <GameObjectId> player_id
  kPlayerRespawned,

  // [0] - <float> experience_to_add
  kIncreaseLocalPlayerExperience,

  kLocalPlayerDied,

  // [0] - <QPointF> point_to_spawn
  kReviveLocalPlayer,

  // [0] - <GameObjectId> player_id
  kReviveConfirmed,

  // [0] - <float> health_points
  kUpdateLocalPlayerHealthPoints,

  // [0] - <GameObjectId> player_id
  kRequestRespawn,

  SIZE
};

Q_ENUM_NS(EventType)

}  // namespace EventTypeWrapper

using EventType = EventTypeWrapper::EventType;
Q_DECLARE_METATYPE(EventType)

#endif  // EVENT_EVENT_TYPE_H_
