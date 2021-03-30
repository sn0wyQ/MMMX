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

Player::Player(GameObjectId player_id, const std::vector<QVariant>& params)
  : Player(player_id, params.at(0).toFloat(), params.at(1).toFloat(),
           params.at(2).toFloat()) {}

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

bool Player::IsFilteredByFov() const {
  return true;
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}

void Player::SetParams(const std::vector<QVariant>& params) {
  SetX(params.at(0).toFloat());
  SetY(params.at(1).toFloat());
  QPointF vec = params.at(2).toPointF();
  SetVelocity(QVector2D(vec.x(), vec.y()));
  SetRotation(params.at(3).toFloat());
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetVelocity());
  result.emplace_back(GetRotation());
  return result;
}
GameObjectType Player::GetGameObjectType() const {
  return GameObjectType::kPlayer;
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
