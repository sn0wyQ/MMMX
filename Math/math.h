#ifndef MATH_MATH_H_
#define MATH_MATH_H_

#include <algorithm>
#include <cmath>
#include <vector>
#include <memory>

#include <QLineF>
#include <QPointF>
#include <QVector2D>

#include "GameObject/RigidBody/intersect_constants.h"

namespace Math {

// Constants
constexpr float kPi = 3.1415927410125732421875f;

constexpr float kDegToRad = kPi / 180.f;
constexpr float kRadToDeg = 180.f / kPi;


// Returns the angle between X axis and line from |start| to |end|
// Angle is measured in degrees, return value is inside [0.f, 360.f)
float VectorAngle(const QPointF& start, const QPointF& end);
QVector2D GetVectorByAngle(float rotation);

float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);

float GetRotationInOtherAxis(float rotation);

bool IsPointOnSegment(QPointF first, QPointF second, QPointF point);

float DistanceBetweenPoints(QPointF first, QPointF second);

float GetRightAngle(float angle);  // always return angle [0; 360)

template <typename T>
std::vector<QPointF> GetRectanglePoints(QPointF position, float rotation,
                                        const std::shared_ptr<T>& object) {
  std::vector<QPointF> points;
  points.emplace_back(- object->GetWidth() / 2.,
                      - object->GetHeight() / 2.);
  points.emplace_back(+ object->GetWidth() / 2.,
                      - object->GetHeight() / 2.);
  points.emplace_back(+ object->GetWidth() / 2.,
                      + object->GetHeight() / 2.);
  points.emplace_back(- object->GetWidth() / 2.,
                      + object->GetHeight() / 2.);
  float rotation_rad = Math::DegreesToRadians(rotation);
  for (auto& point : points) {
    float x = point.x();
    float y = point.y();
    point.setX(x * std::cos(rotation_rad) + y * std::sin(rotation_rad));
    point.setY(-x * std::sin(rotation_rad) + y * std::cos(rotation_rad));
    point.rx() += position.x();
    point.ry() += position.y();
  }
  return points;
}

}  // namespace Math

#endif  // MATH_MATH_H_
