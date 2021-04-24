#include <utility>

#include "player_stats.h"

PlayerStats::PlayerStats(GameObjectId player_id,
                       QString nickname) : player_id_{player_id},
                                           nickname_{std::move(nickname)} {}

void PlayerStats::SetParams(std::vector<QVariant> params) {
  this->SetDeaths(params.back().toInt());
  params.pop_back();
  this->SetKills(params.back().toInt());
  params.pop_back();
  this->SetNickname(params.back().toString());
  params.pop_back();
  player_id_ = params.back().toInt();
  params.pop_back();
}

std::vector<QVariant> PlayerStats::GetParams() const {
  std::vector<QVariant> params;
  params.emplace_back(this->GetPlayerId());
  params.emplace_back(this->GetNickname());
  params.emplace_back(this->GetKills());
  params.emplace_back(this->GetDeaths());
  return params;
}

const QString& PlayerStats::GetNickname() const {
  return nickname_;
}

int PlayerStats::GetKills() const {
  return kills_;
}

int PlayerStats::GetDeaths() const {
  return deaths_;
}

void PlayerStats::SetNickname(const QString& nickname) {
  nickname_ = nickname;
}

void PlayerStats::SetKills(int kills) {
  kills_ = kills;
}

void PlayerStats::SetDeaths(int deaths) {
  deaths_ = deaths;
}

GameObjectId PlayerStats::GetPlayerId() const {
  return player_id_;
}
