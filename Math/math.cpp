#include "Math/math.h"

float Math::VectorAngle(const QPointF& start, const QPointF& end) {
  return QLineF(start, end).angle();
}

float Math::DegreesToRadians(float degrees) {
  return degrees * kDegToRad;
}

float Math::RadiansToDegrees(float radians) {
  return radians * kRadToDeg;
}

bool Math::IsPointOnSegment(QPointF first, QPointF second, QPointF point) {
  float min_x = std::min(first.x(), second.x());
  float max_x = std::max(first.x(), second.x());
  float min_y = std::min(first.y(), second.y());
  float max_y = std::max(first.y(), second.y());
  float eps = IntersectChecker::kEps;
  return (min_x - eps <= point.x() && point.x() <= max_x + eps
      && min_y - eps <= point.y() && point.y() <= max_y + eps);
}

float Math::GetRotationInOtherAxis(float rotation) {
  float new_rotation = 540.f - rotation;
  if (new_rotation >= 360.f) {
    new_rotation -= 360.f;
  }
  return new_rotation;
}

QVector2D Math::GetVectorByAngle(float rotation) {
  float radians = Math::DegreesToRadians(rotation);
  QVector2D result(std::cos(radians), std::sin(radians));
  result.setY(-result.y());
  return result.normalized();
}

float Math::DistanceBetweenPoints(QPointF first, QPointF second) {
  float distance = sqrt((first.x() - second.x()) * ((first.x() - second.x())) +
      (first.y() - second.y()) * ((first.y() - second.y())));
  return distance;
}

float Math::GetNormalizeAngle(float angle) {
  int64_t cnt = static_cast<int64_t>(angle) / 360;
  angle = angle + 360.f * static_cast<float>(cnt);
  return (angle < 0.f ? angle + 360.f : angle);
}
