//
// Created by matvey on 21.04.21.
//

#ifndef PLAYERDATA_PLAYER_DATA_H_
#define PLAYERDATA_PLAYER_DATA_H_

#include <vector>
#include <QVariant>

#include "constants.h"

class PlayerData {
 public:
  PlayerData() = default;
  PlayerData(GameObjectId game_object_id, QString nickname);
  void SetParams(std::vector<QVariant> params);
  std::vector<QVariant> GetParams() const;
  void SetNickname(const QString& nickname);
  void SetKills(int kills);
  void SetDeaths(int deaths);
  const QString& GetNickname() const;
  int GetKills() const;
  int GetDeaths() const;
  GameObjectId GetPlayerId() const;

 private:
  GameObjectId player_id_{Constants::kNullGameObjectId};
  QString nickname_{""};
  int kills_{0};
  int deaths_{0};
};

#endif  // PLAYERDATA_PLAYER_DATA_H_
