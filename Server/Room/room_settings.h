#ifndef SERVER_ROOM_ROOM_SETTINGS_H_
#define SERVER_ROOM_ROOM_SETTINGS_H_

#include "Constants/constants.h"

class RoomSettings {
 public:
  RoomSettings() = default;
  explicit RoomSettings(int max_clients);

  int GetMaxClients() const;
  void SetMaxClients(int max_clients);

 private:
  int max_clients_{Constants::kDefaultMaxClients};
};

#endif  // SERVER_ROOM_ROOM_SETTINGS_H_
