#include "player.h"

Player::Player(GameObjectId player_id)
  : Entity(player_id, std::make_shared<RigidBodyCircle>(1.f)) {}

Player::Player(GameObjectId player_id,
               float x,
               float y,
               float rotation)
    : Player(player_id) {
  SetX(x);
  SetY(y);
  SetRotation(rotation);
}

void Player::OnTick(int time_from_previous_tick) {
  ApplyVelocity(time_from_previous_tick);
}

void Player::DrawRelatively(Painter* painter) {
  // Body [Temporary]
  painter->DrawEllipse(QPointF(), 1.f, 1.f);

  // Eyes [Temporary]
  painter->DrawEllipse(QPointF(0.4f, -0.3f), 0.2f, 0.2f);
  painter->DrawEllipse(QPointF(0.4, 0.3f), 0.2f, 0.2f);

  // Direction Arrow [Needs improvement to look cuter]
  if (IsLocalPlayer()) {
    painter->DrawTriangle(QPointF(1.2f, -0.3f),
                          QPointF(1.2f, 0.3f),
                          QPointF(1.7f, 0.f));
  }
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}
