#ifndef MODEL_CLIENT_GAME_MODEL_H_
#define MODEL_CLIENT_GAME_MODEL_H_

#include <memory>

#include "Model/game_model.h"

enum class BoolVariable {
  kIsInFov,
  SIZE
};

class ClientGameModel : public GameModel {
 public:
  std::shared_ptr<Player> GetLocalPlayer() const;
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

  void AddInterpolateInfo(GameObjectId game_object_id,
                          GameObjectType game_object_type,
                          int64_t server_time);
  std::shared_ptr<GameObject> GetGameObjectByGameObjectIdToBeInterpolated(
        GameObjectId game_object_id) const;

  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>&
    GetInterpolator();

  struct BoolUpdate {
    int64_t update_time;
    bool value;
  };

  void AddScheduledUpdate(GameObjectId game_object_id,
                          BoolVariable bool_variable,
                          const BoolUpdate& bool_update);
  void UpdateScheduledBools(int64_t current_time);


 private:
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
  std::unordered_map<GameObjectId, std::shared_ptr<GameObject>> interpolater_;

  bool GetValueAccordingVariable(GameObjectId game_object_id,
                                 BoolVariable bool_variable);
  void SetValueAccordingVariable(GameObjectId game_object_id,
                                 BoolVariable bool_variable,
                                 bool value);
  std::array<std::unordered_map<GameObjectId, std::deque<BoolUpdate>>,
  static_cast<uint32_t>(BoolVariable::SIZE)>
    scheduled_bool_updates_;
};

#endif  // MODEL_CLIENT_GAME_MODEL_H_
