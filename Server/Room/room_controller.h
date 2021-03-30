#ifndef SERVER_ROOM_ROOM_CONTROLLER_H_
#define SERVER_ROOM_ROOM_CONTROLLER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QDebug>
#include <QString>

#include "Controller/base_controller.h"
#include "GameObject/RectangularStaticObject/box.h"
#include "Model/game_data_model.h"
#include "Server/Room/room_settings.h"
#include "constants.h"

enum class RoomState {
  kGameFinished,
  kGameInProgress,
  kWaitingForClients
};

class RoomController : public BaseController {
 public:
  RoomController(RoomId id, RoomSettings room_settings);
  ~RoomController() override = default;

  QString GetControllerName() const override;

  void AddEventToSend() = delete;

  void AddEventToSendToSingleClient(const Event& event, ClientId client_id);
  void AddEventToSendToClientList(const Event& event,
                                  const std::vector<ClientId>& client_ids);
  void AddEventToSendToAllClients(const Event& event);

  void AddEventToSendToSinglePlayer(const Event& event, GameObjectId player_id);
  void AddEventToSendToPlayerList(const Event& event,
                                  const std::vector<GameObjectId>& player_ids);
  void AddEventToSendToAllPlayers(const Event& event);

  void SendEvent(const Event& event) override;
  void OnTick(int delta_time) override;
  void TickPlayers(int delta_time);

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

  std::vector<Event> ClaimEventsForServer();

  void AddBox(float x, float y, float rotation);
  void AddTree(float x, float y, float radius);
  void ShareObjectsOnMap(GameObjectId player_id);

 private:
  RoomId id_;
  GameDataModel model_;
  RoomSettings room_settings_;
  RoomState room_state_ = RoomState::kWaitingForClients;
  std::unordered_map<ClientId, GameObjectId> player_ids_;

  std::vector<Event> events_for_server_;

  // ------------------- GAME EVENTS -------------------

  void SendControlsEvent(const Event& event) override;
};

#endif  // SERVER_ROOM_ROOM_CONTROLLER_H_
