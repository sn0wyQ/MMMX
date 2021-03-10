#ifndef EVENT_EVENT_TYPE_H_
#define EVENT_EVENT_TYPE_H_

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

  // [0] - addressee_client_id,
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

  // [0] - addressee_client_id,
  // [1] - number_of_players,
  // [2+] - player_ids
  kSharePlayersInRoomIds,

  // No arguments
  kStartGame,
};

#endif  // EVENT_EVENT_TYPE_H_
