#ifndef MODEL_CLIENT_GAME_MODEL_H_
#define MODEL_CLIENT_GAME_MODEL_H_

#include <memory>

#include "Model/game_model.h"

class ClientGameModel : public GameModel {
 public:
  std::shared_ptr<Player> GetLocalPlayer() const;
  bool IsLocalPlayerSet() const;
  void SetLocalPlayerId(GameObjectId player_id);

 private:
  GameObjectId local_player_id_{Constants::kNullGameObjectId};
};

#endif  // MODEL_CLIENT_GAME_MODEL_H_
