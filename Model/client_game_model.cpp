#include "client_game_model.h"

bool ClientGameModel::IsLocalPlayerSet() const {
  return local_player_id_ != Constants::kNullGameObjectId;
}

std::shared_ptr<Player> ClientGameModel::GetLocalPlayer() const {
  if (local_player_id_ == Constants::kNullGameObjectId) {
    throw std::runtime_error("[MODEL] Owner's player_id isn't set...");
  }
  return GameModel::GetPlayerByPlayerId(local_player_id_);
}

void ClientGameModel::SetLocalPlayerId(GameObjectId player_id) {
  local_player_id_ = player_id;
  this->GetLocalPlayer()->SetIsLocalPlayer(true);
}
