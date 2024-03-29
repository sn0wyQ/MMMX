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

std::vector<QPointF> GetRotatedRect(const QRectF& rect, float rotation);

template <typename T>
std::vector<QPointF> GetRectanglePoints(QPointF position, float rotation,
                                        const std::shared_ptr<T>& object) {
  QPointF offset(object->GetWidth() / 2.f,
                 object->GetHeight() / 2.f);
  return GetRotatedRect(QRectF(position - offset, position + offset), rotation);
}

struct Line {
  Line() = default;
  Line(double a_, double b_, double c_);
  Line(QPointF point1_, QPointF point2_);
  double a{0.};
  double b{0.};
  double c{0.};
  QPointF point1;
  QPointF point2;
};

QPointF GetLinesIntersection(const Line& line1, const Line& line2);
double GetDistanceFromPointToLine(const QPointF& point, const Line& line);
bool IsPointOnCircle(const QPointF& point, const QPointF& circle_center,
                   double circle_radius);
bool IsPointOnLine(const QPointF& point, const Line& line);
std::vector<QPointF> GetCircleAndLineIntersections(
    const QPointF& circle_center, double circle_radius, const Line& line);
std::vector<QPointF> GetRectWithLineIntersections(const QRectF& rect,
                                                  const Line& line,
                                                  double angle = 0.f);
// Angle must be in radians
QPointF PointToNewCoordinates(const QPointF& point, double angle);

}  // namespace Math

#endif  // MATH_MATH_H_
