#ifndef MODEL_CLIENT_GAME_MODEL_H_
#define MODEL_CLIENT_GAME_MODEL_H_

#include <deque>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QList>
#include <QVariant>

#include "Model/game_model.h"
#include "Server/Room/room_info.h"

enum class Variable {
  kIsInFov,
  SIZE
};

class ClientGameModel : public GameModel {
 public:
  ClientGameModel() = default;
  std::shared_ptr<Player> GetLocalPlayer() const;
  std::shared_ptr<PlayerStats> GetLocalPlayerStats();
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

  void AddInterpolateInfo(GameObjectId game_object_id,
                          GameObjectType game_object_type,
                          int64_t server_time);

  bool IsGameObjectInInterpolation(GameObjectId game_object_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const;

  const std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
    GetInterpolatorMap() const;

  struct UpdateVariable {
    int64_t update_time;
    QVariant value;
  };

  void RemoveFromInterpolator(GameObjectId game_object_id);
  void AddScheduledUpdate(GameObjectId game_object_id,
                          Variable variable,
                          const UpdateVariable& update);
  void UpdateScheduled(int64_t current_time);
  QVariant GetScheduledVariableValue(GameObjectId game_object_id,
                                     Variable variable) const;

  void AddLocalBullets();

  std::vector<std::shared_ptr<Bullet>> GetLocalBullets() const;
  void DeleteLocalBullet(GameObjectId bullet_id);

  const QList<RoomInfo>& GetRoomsInfo() const;
  void SetRoomsInfo(const QList<QVariant>& rooms_info);

 private:
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> interpolator_;

  QVariant GetValueAccordingVariable(GameObjectId game_object_id,
                                 Variable variable) const;
  void SetValueAccordingVariable(GameObjectId game_object_id,
                                 Variable variable,
                                 const QVariant& value);
  std::array<std::unordered_map<GameObjectId, std::deque<UpdateVariable>>,
  static_cast<uint32_t>(Variable::SIZE)>
    scheduled_updates_;

  std::unordered_map<GameObjectId, std::shared_ptr<Bullet>> local_bullets_;
  int bullet_id_to_set_{1};

  QList<RoomInfo> rooms_info_;
};

#endif  // MODEL_CLIENT_GAME_MODEL_H_