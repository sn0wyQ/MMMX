#ifndef SERVER_ROOM_ROOM_CONTROLLER_H_
#define SERVER_ROOM_ROOM_CONTROLLER_H_

#include <string>
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
  kGameInProgress,
  kWaitingForClients,
};

class RoomController : public BaseController {
 public:
  explicit RoomController(RoomId id, RoomSettings room_settings);
  ~RoomController() override = default;

  QString GetControllerName() const override;

  void SendEvent(const Event& event) override;
  void OnTick() override;

  void AddClient(ClientId client_id);
  void RemoveClient(ClientId client_id);

  bool HasFreeSpot() const;
  bool HasPlayers() const;
  bool IsGameInProgress() const;
  bool IsWaitingForClients() const;

  std::vector<ClientId> GetAllClientsIds() const;
  std::vector<GameObjectId> GetAllPlayerIds() const;
  RoomId GetId() const;

  GameObjectId ClientIdToPlayerId(ClientId client_id) const;
  ClientId PlayerIdToClientId(GameObjectId player_id) const;

  GameObjectId GetNextUnusedPlayerId() const;

  std::vector<Event> ClaimEventsForServer();

 private:
  RoomId id_;
  GameDataModel model_;
  RoomSettings room_settings_;
  RoomState room_state_ = RoomState::kWaitingForClients;
  std::unordered_map<ClientId, GameObjectId> player_ids_;

  std::vector<Event> events_for_server_;

  void AddNewPlayerEvent(const Event& event) override;
  void SharePlayersInRoomInfoEvent(const Event& event) override;
  void StartGameEvent(const Event& event) override;
  void UpdateSvarEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void SendDirectionInfoEvent(const Event& event) override;
};

#endif  // SERVER_ROOM_ROOM_CONTROLLER_H_
