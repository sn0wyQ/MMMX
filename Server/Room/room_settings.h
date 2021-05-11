#ifndef SERVER_ROOM_ROOM_SETTINGS_H_
#define SERVER_ROOM_ROOM_SETTINGS_H_

#include "constants.h"

class RoomSettings {
 public:
  RoomSettings() = default;
  explicit RoomSettings(int max_clients);

  int GetMaxClients() const;
  int GetGameDuration() const;
  int GetWarmupDuration() const;

 private:
  int max_clients_{Constants::kDefaultMaxClients};
  int game_duration_{Constants::kDefaultGameDuration};
  int warmup_duration_{Constants::kDefaultWarmupDuration};
};

#endif  // SERVER_ROOM_ROOM_SETTINGS_H_
