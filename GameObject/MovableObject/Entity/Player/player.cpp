#include "player.h"

Player::Player(GameObjectId player_id, float radius)
  : Entity(player_id, std::make_shared<RigidBodyCircle>(radius)) {}

Player::Player(GameObjectId player_id, float x, float y, float radius)
    : Player(player_id, radius) {
  SetX(x);
  SetY(y);
  SetRadius(radius);
}

Player::Player(GameObjectId player_id, const std::vector<QVariant>& params)
  : Player(player_id, params.at(0).toFloat(), params.at(1).toFloat(),
           params.at(3).toFloat()) {
  QPointF vec = params.at(2).toPointF();
  SetVelocity(QVector2D(vec.x(), vec.y()));
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

void Player::SetParams(const std::vector<QVariant>& params) {
  SetX(params.at(0).toFloat());
  SetY(params.at(1).toFloat());
  QPointF vec = params.at(2).toPointF();
  SetVelocity(QVector2D(vec.x(), vec.y()));
  SetRadius(params.at(3).toFloat());
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetVelocity());
  result.emplace_back(GetRadius());
  return result;
}
GameObjectType Player::GetType() const {
  return GameObjectType::kPlayer;
}

float Player::GetFovRadius() const {
  return fov_radius_;
}

void Player::SetFovRadius(float fov_radius) {
  fov_radius_ = fov_radius;
}

void Player::SetRadius(float radius) {
  SetWidth(radius * 2.f);
  SetHeight(radius * 2.f);
}

float Player::GetRadius() const {
  return GetWidth() / 2.f;
}
