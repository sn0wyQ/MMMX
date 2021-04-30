#ifndef SERVER_ROOM_ROOM_CONTROLLER_H_
#define SERVER_ROOM_ROOM_CONTROLLER_H_

#include <algorithm>
#include <deque>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QDateTime>
#include <QDebug>
#include <QString>

#include "Controller/base_controller.h"
#include "GameObject/RigidBody/object_collision.h"
#include "Model/room_game_model.h"
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

  void AddClient(ClientId client_id);
  void RemoveClient(ClientId client_id);

  bool HasFreeSpot() const;
  bool HasPlayers() const;
  bool IsGameInProgress() const;
  bool IsWaitingForClients() const;

  int GetPlayersCount() const;

  std::vector<ClientId> GetAllClientsIds() const;
  std::vector<GameObjectId> GetAllPlayerIds() const;

  RoomId GetId() const;
  GameObjectId ClientIdToPlayerId(ClientId client_id) const;

  ClientId PlayerIdToClientId(GameObjectId player_id) const;
  std::vector<Event> ClaimEventsForServer();

 private:
  RoomId id_;
  std::shared_ptr<RoomGameModel> model_;
  struct ModelData {
    int delta_time;
    std::shared_ptr<RoomGameModel> model;
  };
  std::deque<ModelData> models_cache_;
  RoomSettings room_settings_;
  RoomState room_state_ = RoomState::kWaitingForClients;
  std::unordered_map<ClientId, GameObjectId> player_ids_;
  std::set<std::pair<GameObjectId, GameObjectId>> is_first_in_fov_of_second_;
  std::vector<Event> events_for_server_;
  int creeps_count_{0};

  void RecalculateModel(const ModelData& model_data);
  void TickObjectsInModel(const ModelData& model_data);
  void ProcessBulletsHits(const ModelData& model_data);
  void DeleteReadyToBeDeletedObjects(const ModelData& model_data);

  GameObjectId AddPlayer();
  void AddBox(float x, float y, float rotation, float width, float height);
  void AddRandomBox(float width, float height);
  void AddTree(float x, float y, float radius);
  void AddRandomTree(float radius);
  void AddCreep(float x, float y);
  std::vector<GameObjectId> AddBullets(GameObjectId parent_id, float x, float y,
                         float rotation,
                         const std::shared_ptr<Weapon>& weapon);
  void AddConstantObjects();
  void AddCreeps();

  Event GetEventOfGameObjectData(GameObjectId game_object_id) const;
  Event GetEventOfGameObjectLeftFov(GameObjectId game_object_id) const;
  bool IsGameObjectInFov(GameObjectId game_object_id,
                         GameObjectId player_id);
  void ForceSendPlayersStatsToPlayer(GameObjectId player_id);
  void SendPlayersStatsToPlayers();
  void SendGameObjectsDataToPlayer(GameObjectId player_id);
  int GetModelIdByTimestamp(int64_t timestamp) const;

  void SendNicknameEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void SendPlayerShootingEvent(const Event& event) override;
  void SendControlsEvent(const Event& event) override;
};

#endif  // SERVER_ROOM_ROOM_CONTROLLER_H_
