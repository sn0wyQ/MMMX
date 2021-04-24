#include "bullet.h"

Bullet::Bullet(GameObjectId bullet_id) : MovableObject(bullet_id) {}

void Bullet::SetParams(std::vector<QVariant> params) {
  SetBulletRange(params.back().toFloat());
  params.pop_back();
  SetBulletSpeed(params.back().toFloat());
  params.pop_back();
  SetBulletDamage(params.back().toFloat());
  params.pop_back();
  float start_position_y = params.back().toFloat();
  params.pop_back();
  float start_position_x = params.back().toFloat();
  params.pop_back();
  SetStartPosition(QPointF(start_position_x, start_position_y));
  GameObjectId parent_id = params.back().toInt();
  SetParentId(parent_id);
  params.pop_back();
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

void Bullet::OnTick(int delta_time) {
  MovableObject::OnTick(delta_time);
  CheckRange();
}

void Bullet::CheckRange() {
  if (Math::DistanceBetweenPoints(GetStartPosition(), GetPosition())
      > GetBulletRange()) {
    SetIsNeedToDelete(true);
  }
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
  painter->DrawEllipse(QPointF(),
                       GetWidth() / 2.f, GetHeight() / 2.f);
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
