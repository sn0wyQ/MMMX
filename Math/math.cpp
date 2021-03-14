#include "Math/math.h"

float Math::DirectionAngle(const QPointF& start, const QPointF& end) {
  return QLineF(start, end).angle();
}
