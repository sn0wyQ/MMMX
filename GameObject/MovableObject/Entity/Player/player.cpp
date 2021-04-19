#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {}

void Player::SetParams(std::vector<QVariant> params) {
  auto weapon_type = static_cast<WeaponType>(params.back().toInt());
  weapon_type_ = weapon_type;
  switch (weapon_type) {
    case WeaponType::kMachineGun: {
      weapon_ = std::make_shared<MachineGun>();
      break;
    }
  }
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result = Entity::GetParams();
  result.emplace_back(static_cast<int>(weapon_type_));
  return result;
}

void Player::DrawRelatively(Painter* painter) {
  // Body [Temporary]
  painter->DrawEllipse(QPointF(), GetRadius(), GetRadius());

  // Eyes [Temporary]
  painter->DrawEllipse(QPointF(0.4f * GetRadius(),
                               -0.3f * GetRadius()),
                       0.2f,
                       0.2f);
  painter->DrawEllipse(
      QPointF(0.4f * GetRadius(), 0.3f * GetRadius()),
      0.2f, 0.2f);

  // Direction Arrow [Needs improvement to look cuter]
  if (IsLocalPlayer()) {
    painter->DrawTriangle(QPointF(GetRadius() + 0.2f, -0.3f),
                          QPointF(GetRadius() + 0.2f, 0.3f),
                          QPointF(GetRadius() + 0.7f, 0.f));
  }
  weapon_->DrawWeapon(painter);
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}

GameObjectType Player::GetType() const {
  return GameObjectType::kPlayer;
}

void Player::SetRadius(float radius) {
  SetWidth(radius * 2.f);
  SetHeight(radius * 2.f);
}

float Player::GetRadius() const {
  return GetWidth() / 2.f;
}
