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
  if (!IsPointOnLine(point, Line(first, second))) {
    return false;
  }
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
  float distance = std::sqrt(
      (first.x() - second.x()) * ((first.x() - second.x()))
      + (first.y() - second.y()) * ((first.y() - second.y())));
  return distance;
}

float Math::GetNormalizeAngle(float angle) {
  return std::fmod(angle, 360.f);
}

std::vector<QPointF> Math::GetRotatedRect(const QRectF& rect, float rotation) {
  std::vector<QPointF> points;
  float angle = DegreesToRadians(rotation);
  points.push_back(rect.center() +
      PointToNewCoordinates(rect.topLeft() - rect.center(), angle));
  points.push_back(rect.center() +
      PointToNewCoordinates(rect.topRight() - rect.center(), angle));
  points.push_back(rect.center() +
      PointToNewCoordinates(rect.bottomRight() - rect.center(), angle));
  points.push_back(rect.center() +
      PointToNewCoordinates(rect.bottomLeft() - rect.center(), angle));
  return points;
}

Math::Line::Line(float a_, float b_, float c_) : a(a_), b(b_), c(c_) {}

Math::Line::Line(QPointF point1_, QPointF point2_) : point1(point1_),
                                                     point2(point2_) {
  float x1 = point1_.x();
  float y1 = point1_.y();
  float x2 = point2_.x();
  float y2 = point2_.y();
  a = y2 - y1;
  b = x1 - x2;
  c = x2 * y1 - x1 * y2;
}

QPointF Math::GetLinesIntersection(const Line& line1, const Line& line2) {
  float a1 = line1.a;
  float b1 = line1.b;
  float c1 = line1.c;
  float a2 = line2.a;
  float b2 = line2.b;
  float c2 = line2.c;

  if (std::fabs(b2 * a1 - b1 * a2) < kEps) {
    return {};
  }

  float x = (b1 * c2 - b2 * c1) / (b2 * a1 - b1 * a2);
  float y;
  if (std::fabs(b1) > kEps) {
    y = -(a1 * x + c1) / b1;
  } else {
    y = -(a2 * x + c2) / b2;
  }
  return {x, y};
}

float Math::GetDistanceFromPointToLine(const QPointF& point,
                                       const Line& line) {
  float a = line.a;
  float b = line.b;
  float c = line.c;
  return std::fabs(a * point.x() + b * point.y() + c) / std::sqrt(a * a + b * b);
}

bool Math::IsPointOnCircle(const QPointF& point, const QPointF& circle_center,
                           float circle_radius) {
  return std::fabs(DistanceBetweenPoints(point, circle_center) - circle_radius)
      < kEps;
}

bool Math::IsPointOnLine(const QPointF& point, const Line& line) {
  return std::fabs(line.a * point.x() + line.b * point.y() + line.c) < kEps;
}

std::vector<QPointF> Math::GetCircleAndLineIntersections(
    const QPointF& circle_center, float circle_radius, const Line& line) {
  float center_x = circle_center.x();
  float center_y = circle_center.y();

  float perp_a = line.b;
  float perp_b = -line.a;
  float perp_c = line.c;

  if (std::fabs(perp_a) > Math::kEps) {
    float temp_x = (-perp_b * center_y - perp_c) / perp_a;
    perp_c -= perp_a * (center_x - temp_x);
  } else {
    float temp_y = (-perp_a * center_x - perp_c) / perp_b;
    perp_c -= perp_b * (center_y - temp_y);
  }

  QPointF tangent_point = GetLinesIntersection(line,
                                               Line(perp_a, perp_b, perp_c));
  float a = line.a;
  float b = line.b;
  float distance_to_line = GetDistanceFromPointToLine(circle_center, line);
  float segment_length =
      std::sqrt(std::fabs(circle_radius * circle_radius - distance_to_line *
          distance_to_line));
  float delta_y = segment_length * sin(atan(-a / b));
  float delta_x =
      std::sqrt(std::fabs(segment_length * segment_length - delta_y * delta_y));
  QPointF res1(tangent_point.x() + delta_x, tangent_point.y() + delta_y);
  QPointF res2(tangent_point.x() - delta_x, tangent_point.y() - delta_y);
  std::vector<QPointF> result;
  if (IsPointOnCircle(res1, circle_center, circle_radius) &&
      IsPointOnSegment(line.point1, line.point2, res1)) {
    result.push_back(res1);
  }
  if (IsPointOnCircle(res2, circle_center, circle_radius) &&
      IsPointOnSegment(line.point1, line.point2, res2)) {
    result.push_back(res2);
  }
  return result;
}

std::vector<QPointF> Math::GetRectWithLineIntersections(const QRectF& rect,
                                                        const Line& line,
                                                        float angle) {
  std::vector<QPointF> answer;
  auto rect_points = GetRotatedRect(rect, angle);
  rect_points.push_back(rect_points[0]);
  for (size_t i = 1; i < rect_points.size(); i++) {
    Math::Line rect_line(rect_points[i - 1], rect_points[i]);
    auto point = GetLinesIntersection(rect_line, line);
    if (!point.isNull() &&
        IsPointOnSegment(rect_line.point1, rect_line.point2, point) &&
        IsPointOnSegment(line.point1, line.point2, point)) {
      answer.push_back(point);
    }
  }
  return answer;
}

QPointF Math::PointToNewCoordinates(const QPointF& point, float angle) {
  return {point.x() * std::cos(angle) + point.y() * std::sin(angle),
          -point.x() * std::sin(angle) + point.y() * std::cos(angle)};
}
