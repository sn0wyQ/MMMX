#ifndef SERVER_ROOM_ROOM_H_
#define SERVER_ROOM_ROOM_H_

#include <map>

#include <QDebug>

#include "Controller/base_controller.h"
#include "Model/game_data_model.h"
#include "constants.h"

enum class RoomState {
  // MUST always stay in alphabet order

  kFinished,
  kInProgress,
  kWaitingForClients,
};

class RoomController : public BaseController {
  Q_OBJECT

 public:
  explicit RoomController(RoomId id,
                          int max_clients = Constants::kDefaultMaxClients);
  ~RoomController() override = default;

  void HandleEvent(const Event& event) override;
  void Send() override;

  void AddClient(ClientId client_id);
  void RemoveClient(ClientId client_id);

  bool HasFreeSpot() const;
  bool IsGameInProgress() const;
  bool IsWaitingForClients() const;

  std::vector<ClientId> GetAllClientsIds() const;
  std::vector<GameObjectId> GetAllPlayerIds() const;
  RoomId GetId() const;

  GameObjectId ClientIdToPlayerId(ClientId client_id);

  Q_SIGNALS:
  void SendEventToServer(const Event& event,
                         std::vector<ClientId> receivers);

 private:
  RoomId id_;
  GameDataModel model_;
  int max_clients_;
  RoomState room_state_ = RoomState::kWaitingForClients;
  std::map<ClientId, GameObjectId> player_ids_;
};

#endif  // ROOM_H_
