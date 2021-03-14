#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {}

Player::Player(GameObjectId player_id,
               float x,
               float y,
               float view_angle)
    : Player(player_id) {
  SetX(x);
  SetY(y);
  SetViewAngle(view_angle);
}

void Player::Draw(Painter* painter) {
  painter->save();

  // Set center to Player's position
  painter->Translate(GetPosition());

  painter->RotateCounterClockWise(GetViewAngle());

  // Body
  painter->DrawEllipse(QPointF(), 1.f, 1.f);

  // Eyes
  painter->DrawEllipse(QPointF(0.4f, -0.3f), 0.2f, 0.2f);
  painter->DrawEllipse(QPointF(0.4, 0.3f), 0.2f, 0.2f);

  // Direction Arrow
  if (IsLocalPlayer()) {
    painter->DrawTriangle(QPointF(1.2f, -0.3f),
                          QPointF(1.2f, 0.3f),
                          QPointF(1.7f, 0.f));
  }

  painter->restore();
}

void Player::ApplyDirection(uint32_t mask) {
  QPointF pos = GetPosition();
  if ((mask >> 3u) & 1u) {
    pos.ry() -= 0.05f;
  }
  if ((mask >> 2u) & 1u) {
    pos.rx() += 0.05f;
  }
  if ((mask >> 1u) & 1u) {
    pos.ry() += 0.05f;
  }
  if ((mask >> 0u) & 1u) {
    pos.rx() -= 0.05f;
  }
  SetPosition(pos);
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}
