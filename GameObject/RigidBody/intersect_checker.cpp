#include "intersect_checker.h"

#include <cmath>

std::vector<QPointF> IntersectChecker::GetIntersectPointsBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation) {
  if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                              std::dynamic_pointer_cast<RigidBodyCircle>(second),
                              offset, rotation);
  } else if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                              std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                              offset, rotation);
  }
  return {};
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(
    const std::shared_ptr<RigidBodyCircle>& circle1,
    const std::shared_ptr<RigidBodyCircle>& circle2,
    QVector2D offset, float) {
  float r1 = circle1->GetRadius();
  float r2 = circle2->GetRadius();
  float a = -2 * offset.x();
  float b = -2 * offset.y();
  float c = offset.x() * offset.x() + offset.y() * offset.y() + r1 * r1 - r2 * r2;

  float x0 = -a * c / (a * a + b * b);
  float y0 = -b * c / (a * a + b * b);
  float formula = c * c - r1 * r1 * (a * a + b * b);
  std::vector<QPointF> result;
  if (std::abs(formula) < kEps) {
    result.emplace_back(x0, y0);
  } else if (formula <= kEps) {
    float d = r1 * r1 - c * c / (a * a + b * b);
    float mult = std::sqrt(d / (a * a + b * b));
    QPointF first_intersect(x0 + b * mult, y0 - a * mult);
    QPointF second_intersect(x0 - b * mult, y0 + a * mult);
    result.push_back(first_intersect);
    result.push_back(second_intersect);
  }
  return result;
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(
    const std::shared_ptr<RigidBodyCircle>& circle,
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    QVector2D offset, float rotation) {
  float r = circle->GetRadius();
  std::vector<QPointF> points;
  points.emplace_back(offset.x() - rectangle->GetWidth() / 2.,
                           offset.y() - rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() + rectangle->GetWidth() / 2.,
                           offset.y() - rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() + rectangle->GetWidth() / 2.,
                           offset.y() + rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() - rectangle->GetWidth() / 2.,
                           offset.y() + rectangle->GetHeight() / 2.);
  QPointF to_be_center = (points[0] + points[2]) / 2.f;
  float rotation_rad = rotation / 180.f * static_cast<float>(M_PI);
  for (auto& point : points) {
    float x = point.x();
    float y = point.y();
    point.setX(x - to_be_center.x());
    point.setY(y - to_be_center.y());
    x = point.x();
    y = point.y();
    point.setX(x * std::cos(rotation_rad) + y * std::sin(rotation_rad));
    point.setY(-x * std::sin(rotation_rad) + y * std::cos(rotation_rad));
    x = point.x();
    y = point.y();
    point.setX(x + to_be_center.x());
    point.setY(y + to_be_center.y());
  }
  std::vector<QPointF> result;
  for (int i = 0; i < 4; i++) {
    QPointF first = points[i];
    QPointF second = points[(i + 1) % 4];
    float a = second.y() - first.y();
    float b = first.x() - second.x();
    float c = first.y() * second.x() - first.x() * second.y();
    float x0 = -a * c / (a * a + b * b);
    float y0 = -b * c / (a * a + b * b);
    float formula = c * c - r * r * (a * a + b * b);
    if (std::abs(formula) < kEps) {
      if (IsPointInSegment(first, second, QPointF(x0, y0))) {
        result.emplace_back(x0, y0);
      }
    } else if (formula <= kEps) {
      float d = r * r - c * c / (a * a + b * b);
      float mult = std::sqrt(d / (a * a + b * b));
      QPointF first_intersect(x0 + b * mult, y0 - a * mult);
      QPointF second_intersect(x0 - b * mult, y0 + a * mult);
      if (IsPointInSegment(first, second, first_intersect)) {
        result.push_back(first_intersect);
      }
      if (IsPointInSegment(first, second, second_intersect)) {
        result.push_back(second_intersect);
      }
    }
  }
  return result;
}

bool IntersectChecker::IsPointInSegment(QPointF first,
                                        QPointF second,
                                        QPointF point) {
  float min_x = std::min(first.x(), second.x());
  float max_x = std::max(first.x(), second.x());
  float min_y = std::min(first.y(), second.y());
  float max_y = std::max(first.y(), second.y());
  if (min_x - kEps <= point.x() && point.x() <= max_x + kEps
    && min_y - kEps <= point.y() && point.y() <= max_y + kEps) {
    return true;
  }
  return false;
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersectBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation, QVector2D delta_intersect) {
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
  return -delta_intersect * l;
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersectBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation,
    QVector2D delta_intersect) {
  float l = 0;
  float r = 1;
  while (r - l > kEps) {
    float m = (l + r) / 2;
    if (GetIntersectPointsBodies(first, second,
                           offset - delta_intersect * m, rotation).size() < 2u) {
      l = m + kEps;
    } else {
      r = m;
    }
  }
  return -delta_intersect * l;
}

bool IntersectChecker::IsSimilarVectors(QVector2D first, QVector2D second) {
  return QVector2D::dotProduct(first, second) > 1 - kCosEps;
}

QVector2D IntersectChecker::GetRotatedVector(
    QVector2D first, QVector2D second, float alpha) {
  float alpha_rad = alpha / 180.f * static_cast<float>(M_PI);
  QVector3D first_3d(first.x(), first.y(), 0);
  QVector3D second_3d(second.x(), second.y(), 0);
  QVector3D vz = QVector3D::crossProduct(first_3d, second_3d);
  vz.normalize();
  QVector3D vy = QVector3D::crossProduct(vz, first_3d);
  QVector3D v = first_3d * std::cos(alpha_rad) + vy * std::sin(alpha_rad);
  return QVector2D(v.x(), v.y());
}
