#ifndef MATH_MATH_H_
#define MATH_MATH_H_

#include <algorithm>
#include <cmath>
#include <vector>
#include <memory>

#include <QLineF>
#include <QPointF>
#include <QVector2D>
#include <QRectF>

#include "GameObject/RigidBody/intersect_constants.h"

namespace Math {
// Constants
constexpr float kPi = 3.1415927410125732421875f;
constexpr float kEps = 1e-6f;

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

// always return angle [0; 360)
float GetNormalizeAngle(float angle);

template <typename T>
std::vector<QPointF> GetRectanglePoints(QPointF position, float rotation,
                                        const std::shared_ptr<T>& object) {
  std::vector<QPointF> points;
  points.emplace_back(- object->GetWidth() / 2.f,
                      - object->GetHeight() / 2.f);
  points.emplace_back(+ object->GetWidth() / 2.f,
                      - object->GetHeight() / 2.f);
  points.emplace_back(+ object->GetWidth() / 2.f,
                      + object->GetHeight() / 2.f);
  points.emplace_back(- object->GetWidth() / 2.f,
                      + object->GetHeight() / 2.f);
  float rotation_rad = Math::DegreesToRadians(rotation);
  for (auto& point : points) {
    auto x = static_cast<float>(point.x());
    auto y = static_cast<float>(point.y());
    point.setX(x * std::cos(rotation_rad) + y * std::sin(rotation_rad));
    point.setY(-x * std::sin(rotation_rad) + y * std::cos(rotation_rad));
    point.rx() += position.x();
    point.ry() += position.y();
  }
  return points;
}

struct Line {
  Line() = default;
  Line(double a_, double b_, double c_);
  Line(QPointF dot1_, QPointF dot2_);
  double a{0.};
  double b{0.};
  double c{0.};
  QPointF dot1;
  QPointF dot2;
};

QPointF GetLinesIntersection(const Line& line1, const Line& line2);
double GetDistanceFromDotToLine(const QPointF& dot, const Line& line);
bool IsDotOnCircle(const QPointF& dot, const QPointF& circle_center,
                   float circle_radius);
bool IsDotOnLine(const QPointF& dot, const Line& line);
std::vector<QPointF> GetCircleAndLineIntersections(
    const QPointF& circle_center, float circle_radius, const Line& line);
std::vector<QPointF> GetRectWithLineIntersections(const QRectF& rect,
                                                  const Line& line,
                                                  float angle = 0.f);
bool IsDotOnSegment(const QPointF& dot, const Line& line);
// Angle must be in radians
QPointF PointToNewCoordinates(const QPointF& point, float angle);

}  // namespace Math

#endif  // MATH_MATH_H_
