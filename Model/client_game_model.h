#ifndef MODEL_CLIENT_GAME_MODEL_H_
#define MODEL_CLIENT_GAME_MODEL_H_

#include <deque>
#include <memory>
#include <unordered_map>

#include "Model/game_model.h"

enum class Variable {
  kIsInFov,
  kVelocity,
  SIZE
};

class ClientGameModel : public GameModel {
 public:
  ClientGameModel() = default;
  std::shared_ptr<Player> GetLocalPlayer() const;
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

  void AddInterpolateInfo(GameObjectId game_object_id,
                          GameObjectType game_object_type,
                          int64_t server_time);

  bool IsGameObjectInInterpolation(GameObjectId game_object_id) const;
  std::shared_ptr<GameObject> GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const;

  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>& Interpolator();

  struct UpdateVariable {
    int64_t update_time;
    QVariant value;
  };

  void AddScheduledUpdate(GameObjectId game_object_id,
                          Variable variable,
                          const UpdateVariable& update);
  void UpdateScheduled(int64_t current_time);


 private:
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> interpolator_;

  QVariant GetValueAccordingVariable(GameObjectId game_object_id,
                                 Variable variable);
  void SetValueAccordingVariable(GameObjectId game_object_id,
                                 Variable variable,
                                 const QVariant& value);
  std::array<std::unordered_map<GameObjectId, std::deque<UpdateVariable>>,
  static_cast<uint32_t>(Variable::SIZE)>
    scheduled_bool_updates_;
};

#endif  // MODEL_CLIENT_GAME_MODEL_H_
