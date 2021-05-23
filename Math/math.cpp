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

Math::Line::Line(double a_, double b_, double c_) : a(a_), b(b_), c(c_) {}

Math::Line::Line(QPointF dot1_, QPointF dot2_) : dot1(dot1_), dot2(dot2_) {
  double x1 = dot1_.x();
  double y1 = dot1_.y();
  double x2 = dot2_.x();
  double y2 = dot2_.y();
  a = y2 - y1;
  b = x1 - x2;
  c = x2 * y1 - x1 * y2;
}

QPointF Math::GetLinesIntersection(const Line& line1, const Line& line2) {
  double a1 = line1.a;
  double b1 = line1.b;
  double c1 = line1.c;
  double a2 = line2.a;
  double b2 = line2.b;
  double c2 = line2.c;

  if(fabs(b2 * a1 - b1 * a2) < kEps) {
    return {};
  }

  double x = (b1 * c2 - b2 * c1) / (b2 * a1 - b1 * a2);
  double y;
  if (fabs(b1) > kEps) {
    y = -(a1 * x + c1) / b1;
  } else {
    y = -(a2 * x + c2) / b2;
  }
  return {x, y};
}

double Math::GetDistanceFromDotToLine(const QPointF& dot, const Line& line) {
  double a = line.a;
  double b = line.b;
  double c = line.c;
  return fabs(a * dot.x() + b * dot.y() + c) / sqrt(a * a + b * b);
}

bool Math::IsDotOnCircle(const QPointF& dot, const QPointF& circle_center,
                         float circle_radius) {
  return fabs(DistanceBetweenPoints(dot, circle_center) - circle_radius) < kEps;
}

bool Math::IsDotOnLine(const QPointF& dot, const Line& line) {
  return fabs(line.a * dot.x() + line.b * dot.y() + line.c) < kEps;
}

std::vector<QPointF> Math::GetCircleAndLineIntersections(
    const QPointF& circle_center, float circle_radius, const Line& line) {
  double center_x = circle_center.x();
  double center_y = circle_center.y();

  double perp_a = line.b;
  double perp_b = -line.a;
  double perp_c = line.c;

  if (fabs(perp_a) > Math::kEps) {
    double temp_x = (-perp_b * center_y - perp_c) / perp_a;
    perp_c -= perp_a * (center_x - temp_x);
  } else {
    double temp_y = (-perp_a * center_x - perp_c) / perp_b;
    perp_c -= perp_b * (center_y - temp_y);
  }

  QPointF tangent_point = GetLinesIntersection(line,
                                               Line(perp_a, perp_b, perp_c));
  double a = line.a;
  double b = line.b;
  double distance_to_line = GetDistanceFromDotToLine(circle_center, line);
  double segment_length =
      sqrt(fabs(circle_radius * circle_radius - distance_to_line *
          distance_to_line));
  double delta_y = segment_length * sin(atan(-a / b));
  double delta_x =
      sqrt(fabs(segment_length * segment_length - delta_y * delta_y));
  QPointF res1(tangent_point.x() + delta_x, tangent_point.y() + delta_y);
  QPointF res2(tangent_point.x() - delta_x, tangent_point.y() - delta_y);
  std::vector<QPointF> result;
  if (IsDotOnCircle(res1, circle_center, circle_radius) &&
      IsDotOnSegment(res1, line)) {
    result.push_back(res1);
  }
  if (IsDotOnCircle(res2, circle_center, circle_radius) &&
      IsDotOnSegment(res2, line)) {
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
    auto dot = GetLinesIntersection(rect_line, line);
    if (!dot.isNull() && IsDotOnSegment(dot, rect_line) &&
        IsDotOnSegment(dot, line)) {
      answer.push_back(dot);
    }
  }
  return answer;
}

bool Math::IsDotOnSegment(const QPointF& dot, const Line& line) {
  if (!IsDotOnLine(dot, Line(line.dot1, line.dot2))) {
    return false;
  }
  auto start_dot = line.dot1;
  auto finish_dot = line.dot2;
  auto is_less_or_equal = [] (double a, double b) {
    return (a < b) || fabs(a - b) < kEps;
  };
  return (is_less_or_equal(std::min(start_dot.x(), finish_dot.x()), dot.x()) &&
      is_less_or_equal(dot.x(), std::max(start_dot.x(), finish_dot.x())) &&
      is_less_or_equal(std::min(start_dot.y(), finish_dot.y()), dot.y()) &&
      is_less_or_equal(dot.y(), std::max(start_dot.y(), finish_dot.y())));
}

QPointF Math::PointToNewCoordinates(const QPointF& point, float angle) {
  return {point.x() * std::cos(angle) + point.y() * std::sin(angle),
          -point.x() * std::sin(angle) + point.y() * std::cos(angle)};
}
