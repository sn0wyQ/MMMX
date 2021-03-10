#include "room_settings.h"

RoomSettings::RoomSettings(int max_clients) : max_clients_(max_clients) {}

int RoomSettings::GetMaxClients() const {
  return max_clients_;
}

void RoomSettings::SetMaxClients(int max_clients) {
  max_clients_ = max_clients;
}
