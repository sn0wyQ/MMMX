#ifndef SERVER_ROOM_ROOM_CONTROLLER_H_
#define SERVER_ROOM_ROOM_CONTROLLER_H_

#include <unordered_map>
#include <utility>
#include <vector>

#include <QDebug>

#include "Controller/base_controller.h"
#include "Model/game_data_model.h"
#include "Server/Room/room_settings.h"
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

  void SendEvent(const Event& event) override;

  void AddClient(ClientId client_id);
  void RemoveClient(ClientId client_id);

  bool HasFreeSpot() const;
  bool IsGameInProgress() const;
  bool IsWaitingForClients() const;

  std::vector<ClientId> GetAllClientsIds() const;
  std::vector<GameObjectId> GetAllPlayerIds() const;
  RoomId GetId() const;

  GameObjectId ClientIdToPlayerId(ClientId client_id) const;

  GameObjectId GetNextUnusedPlayerId() const;

  Q_SIGNALS:
  void SendEventToServer(const Event& event,
                         std::vector<ClientId> receivers);

 private:
  RoomId id_;
  GameDataModel model_;
  RoomSettings room_settings_;
  RoomState room_state_ = RoomState::kWaitingForClients;
  std::unordered_map<ClientId, GameObjectId> player_ids_;

  void AddNewPlayerEvent(const Event& event) override;
  void PressedTestButtonEvent(const Event& event) override;
  void SharePlayersInRoomIdsEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
};

#endif  // SERVER_ROOM_ROOM_CONTROLLER_H_
