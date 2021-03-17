#ifndef MATH_MATH_H_
#define MATH_MATH_H_

#include <QLineF>
#include <QPointF>

namespace Math {
  // Constants
  constexpr float kPi = 3.1415927410125732421875f;

  // Returns the angle between X axis and line from |start| to |end|
  // Angle is measured in degrees, return value is inside [0.f, 360.f)
  float DirectionAngle(const QPointF& start, const QPointF& end);
}  // namespace Math

#endif  // MATH_MATH_H_
