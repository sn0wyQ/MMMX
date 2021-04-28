#include <utility>

#include "player_stats.h"

PlayerStats::PlayerStats(GameObjectId player_id,
                       QString nickname, int level) : player_id_{player_id},
                                           nickname_{std::move(nickname)},
                                           level_(level) {}

void PlayerStats::SetParams(std::vector<QVariant> params) {
  this->SetLevel(params.back().toInt());
  params.pop_back();
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
  params.emplace_back(this->GetLevel());
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

int& PlayerStats::GetMutableDeaths() {
  return deaths_;
}

GameObjectId PlayerStats::GetPlayerId() const {
  return player_id_;
}

int PlayerStats::GetLevel() const {
  return level_;
}

void PlayerStats::SetLevel(int level) {
  level_ = level;
}

bool PlayerStats::operator<(const PlayerStats& other) const {
  if (this->GetLevel() == other.GetLevel()) {
    if (this->GetKills() == other.GetKills()) {
      if (this->GetDeaths() == other.GetDeaths()) {
        return this->GetNickname() < other.GetNickname();
      }
      return this->GetDeaths() < other.GetDeaths();
    }
    return this->GetKills() > other.GetKills();
  }
  return this->GetLevel() > other.GetLevel();
}
