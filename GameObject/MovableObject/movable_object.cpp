#include "movable_object.h"

MovableObject::MovableObject(GameObjectId game_object_id)
    : GameObject(game_object_id) {}

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
  return position / GetCurrentSpeed()
  / static_cast<float>(delta_time);
}

QVector2D MovableObject::GetAppliedDeltaPosition(
    int delta_time) const {
  return GetVelocity() * GetCurrentSpeed()
    * static_cast<float>(delta_time);
}

void MovableObject::ApplyVelocity(int delta_time) {
  QPointF pos = GetPosition();
  pos += GetAppliedDeltaPosition(delta_time).toPointF();
  SetPosition(pos);
}

float MovableObject::GetCurrentSpeed() const {
  // Temporary code
  return Constants::kDefaultMovableObjectSpeed;
}

void MovableObject::OnTick(int delta_time) {
  ApplyVelocity(delta_time);
}

bool MovableObject::IsMovable() const {
  return true;
}

void MovableObject::SetParams(std::vector<QVariant> params) {
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
  return result;
}

bool MovableObject::IsFilteredByFov() const {
  return true;
}

float MovableObject::GetShortestDistance(
    const std::shared_ptr<GameObject>& object) {
  // Здесь учитываем, что мувабл - всегда круг
  std::vector<QPointF> points =
      Math::GetRectanglePoints(object->GetPosition(), object->GetRotation(),
                               object);
  float min_distance =
      QLineF(this->GetPosition(), object->GetPosition()).length();
  for (int i = 0; i < 4; i++) {
    QPointF first = points[i];
    QPointF second = points[(i + 1) % 4];
    min_distance = std::min(
        min_distance,
        static_cast<float>(QLineF(this->GetPosition(), first).length()));
    // Формула нахождения расстояния между точкой и отрезком
    float a_x = static_cast<float>(second.x() - first.x());
    float a_y = static_cast<float>(second.y() - first.y());
    float n_x = -a_y;
    float n_y = a_x;
    auto a = static_cast<float>(second.y() - first.y());
    auto b = static_cast<float>(first.x() - second.x());
    auto c
        = static_cast<float>(first.y() * second.x() - first.x() * second.y());
    float x_0 = this->GetX();
    float y_0 = this->GetY();
    float t = (-a * x_0 - b * y_0 - c) / (a * n_x + b * n_y);
    float x_1 = x_0 + n_x * t;
    float y_1 = y_0 + n_y * t;
    QPointF point(x_1, y_1);
    if (Math::IsPointInSegment(first, second, point)) {
      min_distance = std::min(
          min_distance,
          static_cast<float>(QLineF(this->GetPosition(), point).length()));
    }
  }
  return min_distance;
}
