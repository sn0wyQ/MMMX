#include "bullet.h"

Bullet::Bullet(GameObjectId bullet_id) : MovableObject(bullet_id) {}

void Bullet::SetParams(std::vector<QVariant> params) {
  float start_position_y = params.back().toFloat();
  params.pop_back();
  float start_position_x = params.back().toFloat();
  params.pop_back();
  SetStartPosition(QPointF(start_position_x, start_position_y));
  GameObjectId parent_id = params.back().toInt();
  params.pop_back();
  SetParentId(parent_id);
  MovableObject::SetParams(params);
}

std::vector<QVariant> Bullet::GetParams() const {
  std::vector<QVariant> result = MovableObject::GetParams();
  result.emplace_back(parent_id_);
  result.emplace_back(start_position_.x());
  result.emplace_back(start_position_.y());
  return result;
}

GameObjectType Bullet::GetType() const {
  return GameObjectType::kBullet;
}

GameObjectId Bullet::GetParentId() const {
  return parent_id_;
}

void Bullet::SetParentId(GameObjectId parent_id) {
  parent_id_ = parent_id;
}

QPointF Bullet::GetStartPosition() const {
  return start_position_;
}

void Bullet::SetStartPosition(QPointF start_position) {
  start_position_ = start_position;

}

void Bullet::DrawRelatively(Painter* painter) {
  painter->DrawEllipse(QPointF(), 0.2f, 0.2f);
}
