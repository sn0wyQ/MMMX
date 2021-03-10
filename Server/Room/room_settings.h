#ifndef ROOM_ROOM_SETTINGS_H_
#define ROOM_ROOM_SETTINGS_H_

class RoomSettings {
 public:
  explicit RoomSettings(int max_clients);

  int GetMaxClients() const;
  void SetMaxClients(int max_clients);

 private:
  int max_clients_;
};

#endif  // ROOM_ROOM_SETTINGS_H_
