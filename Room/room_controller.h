#ifndef ROOM_ROOM_CONTROLLER_H_
#define ROOM_ROOM_CONTROLLER_H_

#include <algorithm>
#include <deque>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

#include <QDateTime>
#include <QDebug>
#include <QString>

#include "Controller/base_controller.h"
#include "GameObject/MovableObject/Entity/Creep/creep_settings.h"
#include "GameObject/RigidBody/object_collision.h"
#include "room_game_model.h"
#include "room_info.h"
#include "room_settings.h"
#include "Constants/constants.h"

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

  void AddClient(
      ClientId client_id, const QString& nickname, PlayerType player_type);
  void RemoveClient(ClientId client_id);

  bool HasFreeSpot() const;
  bool HasPlayers() const;
  bool IsGameInProgress() const;
  bool IsWaitingForClients() const;
  bool IsPublic() const;

  void SetRoomState(RoomState room_state);
  void StartGame();

  int GetPlayersCount() const;

  std::vector<ClientId> GetAllClientsIds() const;
  std::vector<GameObjectId> GetAllPlayerIds() const;

  RoomId GetId() const;
  GameObjectId ClientIdToPlayerId(ClientId client_id) const;

  ClientId PlayerIdToClientId(GameObjectId player_id) const;
  std::vector<Event> ClaimEventsForServer();

  RoomInfo GetRoomInfo() const;

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
  std::vector<Event> events_for_server_;
  int creeps_count_{0};
  std::queue<std::pair<GameObjectId, int64_t>> revive_player_at_;
  std::unordered_map<GameObjectId, bool> are_controls_blocked_;

  void RecalculateModel(const ModelData& model_data);
  void RevivePlayers(const ModelData& model_data);
  void TickObjectsInModel(const ModelData& model_data);
  void ProcessBulletHits(
      const RoomController::ModelData& model_data_bullet,
      const std::shared_ptr<Bullet>& bullet,
      const std::vector<std::shared_ptr<GameObject>>& game_objects);
  void ProcessBulletsHits(const ModelData& model_data);
  void TickCreepsIntelligence(const ModelData& model_data);
  void DeleteReadyToBeDeletedObjects(const ModelData& model_data);
  void EntityReceiveDamage(const ModelData& model_data,
                           const std::shared_ptr<Entity>& killer,
                           const std::shared_ptr<Entity>& entity,
                           float damage, bool* is_killed);

  GameObjectId AddPlayer(PlayerType player_type);
  void AddGarage(float x, float y, float rotation, float width, float height);
  void AddRandomGarage(float width, float height);
  void AddTree(float x, float y, float radius);
  void AddRandomTree(float radius);
  void AddCreep(float x, float y);

  std::vector<GameObjectId> AddBullets(
      const std::shared_ptr<RoomGameModel>& model, GameObjectId parent_id,
      float x, float y, float rotation, const std::shared_ptr<Weapon>& weapon,
      const QList<QVariant>& random_bullet_shifts);
  void AddConstantObjects();
  void AddCreeps();

  Event GetEventOfGameObjectData(GameObjectId game_object_id) const;
  Event GetEventOfDeleteGameObject(GameObjectId game_object_id) const;
  void ForceSendPlayersStatsToPlayer(GameObjectId player_id);
  void SendPlayersStatsToPlayers();
  void SendGameObjectsDataToPlayer(GameObjectId player_id,
                                   bool force_sending = false);
  int GetModelIdByTimestamp(int64_t timestamp) const;

  void SendNicknameEvent(const Event& event) override;

  // ------------------- GAME EVENTS -------------------

  void SendControlsEvent(const Event& event) override;
  void SendPlayerShootingEvent(const Event& event) override;
  void SendPlayerReloadingEvent(const Event& event) override;
  void SendLevelingPointsEvent(const Event& event) override;
  void ReviveConfirmedEvent(const Event& event) override;
  void RequestRespawnEvent(const Event& event) override;
};

#endif  // ROOM_ROOM_CONTROLLER_H_
