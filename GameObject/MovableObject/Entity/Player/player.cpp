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

void Player::OnTick(int delta_time) {
  ApplyVelocity(delta_time);
}

void Player::Draw(Painter* painter) {
  painter->save();

  // Set center to Player's position
  painter->Translate(GetPosition());

  painter->RotateCounterClockWise(GetViewAngle());

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

  painter->restore();
}

bool Player::IsFilteredByFov() const {
  return true;
}

// 0th mask bit - IsNeedToGoLeft
// 1st mask bit - IsNeedToGoDown
// 2nd mask bit - IsNeedToGoRight
// 3rd mask bit - IsNeedToGoUp
void Player::UpdateVelocity(uint32_t mask) {
  // Normalizing of (0, 0) vector works bad
  if (mask == 0b0000) {
    SetVelocity({0.f, 0.f});
    return;
  }

  QVector2D velocity{0.f, 0.f};
  if (mask & 0b1000) {
    velocity.setY(-1.f);
  }
  if (mask & 0b0100) {
    velocity.setX(1.f);
  }
  if (mask & 0b0010) {
    velocity.setY(1.f);
  }
  if (mask & 0b0001) {
    velocity.setX(-1.f);
  }

  SetVelocity(velocity.normalized());
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}

float Player::GetFovRadius() const {
  return fov_radius_;
}

void Player::SetFovRadius(float fov_radius) {
  fov_radius_ = fov_radius;
}

bool Player::IsInFov() const {
  return is_in_fov_;
}

void Player::SetIsInFov(bool is_in_fov) {
  is_in_fov_ = is_in_fov;
}
