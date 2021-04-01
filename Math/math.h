#ifndef MATH_MATH_H_
#define MATH_MATH_H_

#include <cmath>
#include <vector>
#include <memory>

#include <QLineF>
#include <QPointF>

#include "GameObject/RigidBody/intersect_constants.h"

namespace Math {
  // Constants
  constexpr float kPi = 3.1415927410125732421875f;

  // Returns the angle between X axis and line from |start| to |end|
  // Angle is measured in degrees, return value is inside [0.f, 360.f)
  float DirectionAngle(const QPointF& start, const QPointF& end);

  float DegreesToRadians(float degrees);

  bool IsPointInSegment(QPointF first, QPointF second, QPointF point);

  template <typename T>
  std::vector<QPointF> GetRectanglePoints(QPointF position, float rotation,
                                          const std::shared_ptr<T>& object) {
    std::vector<QPointF> points;
    points.emplace_back(position.x() - object->GetWidth() / 2.,
                        position.y() - object->GetHeight() / 2.);
    points.emplace_back(position.x() + object->GetWidth() / 2.,
                        position.y() - object->GetHeight() / 2.);
    points.emplace_back(position.x() + object->GetWidth() / 2.,
                        position.y() + object->GetHeight() / 2.);
    points.emplace_back(position.x() - object->GetWidth() / 2.,
                        position.y() + object->GetHeight() / 2.);
    float rotation_rad = Math::DegreesToRadians(rotation);
    for (auto& point : points) {
      float x = point.x();
      float y = point.y();
      point.setX(x - position.x());
      point.setY(y - position.y());
      x = point.x();
      y = point.y();
      point.setX(x * std::cos(rotation_rad) + y * std::sin(rotation_rad));
      point.setY(-x * std::sin(rotation_rad) + y * std::cos(rotation_rad));
      x = point.x();
      y = point.y();
      point.setX(x + position.x());
      point.setY(y + position.y());
    }
    return points;
  }

}  // namespace Math

#endif  // MATH_MATH_H_
