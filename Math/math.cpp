#include "Math/math.h"

float Math::DirectionAngle(const QPointF& start, const QPointF& end) {
  return QLineF(start, end).angle();
}

float Math::DegreesToRadians(float degrees) {
  return degrees / 180.f * static_cast<float>(M_PI);
}
