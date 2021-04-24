//
// Created by matvey on 21.04.21.
//

#include <utility>

#include "player_data.h"

PlayerData::PlayerData(GameObjectId player_id,
                       QString nickname) : player_id_{player_id},
                                           nickname_{std::move(nickname)} {}

void PlayerData::SetParams(std::vector<QVariant> params) {
  player_id_ = params.back().toInt();
  params.pop_back();
  this->SetNickname(params.back().toString());
  params.pop_back();
  this->SetKills(params.back().toInt());
  params.pop_back();
  this->SetDeaths(params.back().toInt());
  params.pop_back();
}

std::vector<QVariant> PlayerData::GetParams() const {
  std::vector<QVariant> params;
  params.emplace_back(this->GetDeaths());
  params.emplace_back(this->GetKills());
  params.emplace_back(this->GetNickname());
  params.emplace_back(player_id_);
  return params;
}

const QString& PlayerData::GetNickname() const {
  return nickname_;
}

int PlayerData::GetKills() const {
  return kills_;
}

int PlayerData::GetDeaths() const {
  return deaths_;
}

void PlayerData::SetNickname(const QString& nickname) {
  nickname_ = nickname;
}

void PlayerData::SetKills(int kills) {
  kills_ = kills;
}

void PlayerData::SetDeaths(int deaths) {
  deaths_ = deaths;
}

GameObjectId PlayerData::GetPlayerId() const {
  return player_id_;
}
