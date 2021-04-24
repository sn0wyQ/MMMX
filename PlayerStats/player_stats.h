#ifndef PLAYERSTATS_PLAYER_STATS_H_
#define PLAYERSTATS_PLAYER_STATS_H_

#include <vector>
#include <QVariant>

#include "constants.h"

class PlayerStats {
 public:
  PlayerStats() = default;
  PlayerStats(GameObjectId game_object_id, QString nickname);

  GameObjectId GetPlayerId() const;
  void SetParams(std::vector<QVariant> params);
  std::vector<QVariant> GetParams() const;

  void SetNickname(const QString& nickname);
  const QString& GetNickname() const;

  void SetKills(int kills);
  int GetKills() const;

  void SetDeaths(int deaths);
  int GetDeaths() const;

  void SetLevel(int level);
  int GetLevel() const;

 private:
  GameObjectId player_id_{Constants::kNullGameObjectId};
  QString nickname_{""};
  int kills_{0};
  int deaths_{0};
  int level_{0};
};

#endif  // PLAYERSTATS_PLAYER_STATS_H_
