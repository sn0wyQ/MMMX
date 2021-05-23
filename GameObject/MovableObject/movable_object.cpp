#include "movable_object.h"

MovableObject::MovableObject(GameObjectId game_object_id)
    : GameObject(game_object_id) {}

MovableObject::MovableObject(const MovableObject& other)
  : GameObject(other) {
  velocity_ = other.velocity_;
  speed_multiplier_ = other.speed_multiplier_;
}

void MovableObject::SetParams(std::vector<QVariant> params) {
  SetSpeedMultiplier(params.back().toFloat());
  params.pop_back();
  float vel_y = params.back().toFloat();
  params.pop_back();
  float vel_x = params.back().toFloat();
  params.pop_back();
  SetVelocity(QVector2D(vel_x, vel_y));
  GameObject::SetParams(params);
}

std::vector<QVariant> MovableObject::GetParams() const {
  std::vector<QVariant> result = GameObject::GetParams();
  result.emplace_back(velocity_.x());
  result.emplace_back(velocity_.y());
  result.emplace_back(GetSpeedMultiplier());
  return result;
}

void MovableObject::OnTick(int delta_time) {
  ApplyVelocity(delta_time);
}

void MovableObject::ApplyVelocity(int delta_time) {
  QPointF pos = GetPosition();
  pos += GetAppliedDeltaPosition(delta_time).toPointF();
  SetPosition(pos);
}

QVector2D MovableObject::GetVelocity() const {
  return velocity_;
}

void MovableObject::SetVelocity(const QVector2D& velocity) {
  velocity_ = velocity;
}

QVector2D MovableObject::GetVelocityByDeltaPosition(
    QVector2D position, int delta_time) const {
  if (delta_time == 0) {
    return QVector2D(0.f, 0.f);
  }
  return position / GetSpeedMultiplier() / static_cast<float>(delta_time);
}

QVector2D MovableObject::GetAppliedDeltaPosition(
    int delta_time) const {
  return GetVelocity() * GetSpeedMultiplier()
    * static_cast<float>(delta_time);
}

bool MovableObject::IsMovable() const {
  return true;
}

bool MovableObject::IsFilteredByFov() const {
  return true;
}

std::shared_ptr<GameObject> MovableObject::Clone() const {
  return std::make_shared<MovableObject>(*this);
}

float MovableObject::GetSpeedMultiplier() const {
  return speed_multiplier_;
}

void MovableObject::SetSpeedMultiplier(float speed_multiplier) {
  speed_multiplier_ = speed_multiplier;
}

void MovableObject::UpdateAnimationState(bool restart) {
  if (velocity_.length() > Math::kEps) {
    this->SetAnimationState(AnimationState::kMove, restart);
  } else {
    this->SetAnimationState(AnimationState::kIdle, restart);
  }
}
