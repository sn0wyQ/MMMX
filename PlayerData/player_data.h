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

  GameObjectId GetPlayerId() const;
  void SetParams(std::vector<QVariant> params);
  std::vector<QVariant> GetParams() const;

  void SetNickname(const QString& nickname);
  const QString& GetNickname() const;

  void SetKills(int kills);
  int GetKills() const;

  void SetDeaths(int deaths);
  int GetDeaths() const;

 private:
  GameObjectId player_id_{Constants::kNullGameObjectId};
  QString nickname_{""};
  int kills_{0};
  int deaths_{0};
};

#endif  // PLAYERDATA_PLAYER_DATA_H_
