#include "room_settings.h"

RoomSettings::RoomSettings(int max_clients) : max_clients_(max_clients) {}

int RoomSettings::GetMaxClients() const {
  return max_clients_;
}

int RoomSettings::GetGameDuration() const {
  return game_duration_;
}

int RoomSettings::GetWarmupDuration() const {
  return warmup_duration_;
}
