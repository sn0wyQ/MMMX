#include "bullet.h"

Bullet::Bullet(GameObjectId bullet_id) : MovableObject(bullet_id) {}

void Bullet::SetParams(std::vector<QVariant> params) {
  auto bullet_range = params.back().toFloat();
  SetBulletRange(bullet_range);
  params.pop_back();
  auto bullet_speed = params.back().toFloat();
  SetBulletSpeed(bullet_speed);
  params.pop_back();
  auto bullet_damage = params.back().toFloat();
  SetBulletDamage(bullet_damage);
  params.pop_back();
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
  result.emplace_back(GetParentId());
  result.emplace_back(static_cast<float>(GetStartPosition().x()));
  result.emplace_back(static_cast<float>(GetStartPosition().y()));
  result.emplace_back(GetBulletDamage());
  result.emplace_back(GetBulletSpeed());
  result.emplace_back(GetBulletRange());
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

std::shared_ptr<GameObject> Bullet::Clone() const {
  return std::make_shared<Bullet>(*this);
}

float Bullet::GetBulletDamage() const {
  return bullet_damage_;
}

float Bullet::GetBulletSpeed() const {
  return bullet_speed_;
}

float Bullet::GetBulletRange() const {
  return bullet_range_;
}

void Bullet::SetBulletDamage(float bullet_damage) {
  bullet_damage_ = bullet_damage;
}

void Bullet::SetBulletSpeed(float bullet_speed) {
  bullet_speed_ = bullet_speed;
}

void Bullet::SetBulletRange(float bullet_range) {
  bullet_range_ = bullet_range;
}
