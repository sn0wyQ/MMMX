#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {}


Player::Player(const Player& other) : Entity(other) {
  is_local_player_ = other.is_local_player_;
}

void Player::SetParams(std::vector<QVariant> params) {
  Entity::SetParams(params);
}

std::vector<QVariant> Player::GetParams() const {
  return Entity::GetParams();
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

std::shared_ptr<GameObject> Player::Clone() const {
  return std::make_shared<Player>(*this);
}
