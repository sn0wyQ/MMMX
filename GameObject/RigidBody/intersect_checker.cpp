#include "intersect_checker.h"

std::vector<QPointF> IntersectChecker::GetIntersectPointsBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation) {
  if (second->GetType() == RigidBodyType::kRectangle) {
    return GetIntersectPoints(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyRectangle>(second),
        offset, rotation);
  } else {
    return GetIntersectPoints(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyCircle>(second),
        offset, rotation);
  }
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(
    const std::shared_ptr<RigidBodyCircle>& circle1,
    const std::shared_ptr<RigidBodyCircle>& circle2,
    QVector2D offset, float) {
  // Формула нахождения точек пересечения двух кругов:
  // http://e-maxx.ru/algo/circles_intersection
  float r1 = circle1->GetRadius();
  float r2 = circle2->GetRadius();
  float a = -2 * offset.x();
  float b = -2 * offset.y();
  float c = offset.x() * offset.x() + offset.y() * offset.y()
      + r1 * r1 - r2 * r2;
  return GetLineWithCircleIntersectPoints(a, b, c, r1);
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(
    const std::shared_ptr<RigidBodyCircle>& circle,
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    QVector2D offset, float rotation) {
  // Нахождение всех точек пересечения круга с прямоугольником -
  // это нахождение точек пересечения с каждой стороной круга
  float r = circle->GetRadius();
  std::vector<QPointF> points = Math::GetRectanglePoints(
      offset.toPointF(), rotation, rectangle);
  std::vector<QPointF> result;
  for (int i = 0; i < 4; i++) {
    QPointF first = points[i];
    QPointF second = points[(i + 1) % 4];
    auto a = static_cast<float>(second.y() - first.y());
    auto b = static_cast<float>(first.x() - second.x());
    auto c
      = static_cast<float>(first.y() * second.x() - first.x() * second.y());
    std::vector<QPointF> points_on_line
      = GetLineWithCircleIntersectPoints(a, b, c, r);
    for (const auto& point : points_on_line) {
      if (Math::IsPointInSegment(first, second, point)) {
        result.push_back(point);
      }
    }
  }
  return result;
}

std::vector<QPointF> IntersectChecker::GetLineWithCircleIntersectPoints(
    float a, float b, float c, float r) {
  // Формула нахождения точек пересечения линии с кругом:
  // http://e-maxx.ru/algo/circle_line_intersection
  std::vector<QPointF> result;
  float x0 = -a * c / (a * a + b * b);
  float y0 = -b * c / (a * a + b * b);
  float formula = c * c - r * r * (a * a + b * b);
  if (std::abs(formula) < kEps) {
    result.emplace_back(x0, y0);
  } else if (formula < -kEps) {
    float d = r * r - c * c / (a * a + b * b);
    float mult = std::sqrt(d / (a * a + b * b));
    QPointF first_intersect(x0 + b * mult, y0 - a * mult);
    result.push_back(first_intersect);
    QPointF second_intersect(x0 - b * mult, y0 + a * mult);
    result.push_back(second_intersect);
  }
  return result;
}

bool IntersectChecker::IsSimilarVectors(QVector2D first, QVector2D second) {
  float cos_between_vectors = QVector2D::dotProduct(first, second)
      / first.length() / second.length();
  return cos_between_vectors >= 1 - kCosEps;
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersectBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation, QVector2D delta_intersect) {
  // Диха максимального расстояния без пересечения с объектом
  float l = 0;
  float r = 1;
  while (r - l > kEps) {
    float m = (l + r) / 2;
    if (!GetIntersectPointsBodies(first, second,
                            offset - delta_intersect * m, rotation).empty()) {
      r = m - kEps;
    } else {
      l = m;
    }
  }
  return delta_intersect * l;
}
