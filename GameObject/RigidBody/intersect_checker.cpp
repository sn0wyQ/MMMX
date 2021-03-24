#include "intersect_checker.h"

std::vector<QPointF> IntersectChecker::GetIntersectPoints(
    const std::shared_ptr<RigidBodyCircle>& circle,
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    QVector2D offset) {
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

std::vector<QPointF> IntersectChecker::GetIntersectPoints(const std::shared_ptr<RigidBodyRectangle>& rectangle,
                                   const std::shared_ptr<RigidBodyCircle>& circle,
                                   QVector2D offset) {
  return GetIntersectPoints(circle, rectangle, -offset);
}

std::vector<QPointF> IntersectChecker::GetIntersectPointsBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset) {
  if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                       std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyRectangle>(first),
                       std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                       std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return GetIntersectPoints(std::dynamic_pointer_cast<RigidBodyRectangle>(first),
                       std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset);
  }
  return {};
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(const std::shared_ptr<RigidBodyCircle>& circle1,
                                   const std::shared_ptr<RigidBodyCircle>& circle2,
                                   QVector2D offset) {
  return {};
}

std::vector<QPointF> IntersectChecker::GetIntersectPoints(const std::shared_ptr<RigidBodyRectangle>& rectangle1,
                                   const std::shared_ptr<RigidBodyRectangle>& rectangle2,
                                   QVector2D offset) {
  return {};
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
    QVector2D offset, QVector2D delta_intersect) {
  if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return -CalculateDistanceToObjectNotToIntersect(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset, delta_intersect);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return -CalculateDistanceToObjectNotToIntersect(
        std::dynamic_pointer_cast<RigidBodyRectangle>(first),
        std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset, delta_intersect);
  } else if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return -CalculateDistanceToObjectNotToIntersect(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset, delta_intersect);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return -CalculateDistanceToObjectNotToIntersect(
        std::dynamic_pointer_cast<RigidBodyRectangle>(first),
        std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset, delta_intersect);
  }
  return QVector2D();
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersect(
    const std::shared_ptr<RigidBodyCircle>& circle,
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    QVector2D offset, QVector2D delta_intersect) {
  float l = 0;
  float r = 1;
  while (r - l > kEps) {
    float m = (l + r) / 2;
    if (!GetIntersectPoints(circle, rectangle,
                            offset - delta_intersect * m).empty()) {
      r = m - kEps;
    } else {
      l = m;
    }
  }
  return -delta_intersect * l;
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersectBodies(const std::shared_ptr<
    RigidBody>& first,
                                                                        const std::shared_ptr<
                                                                            RigidBody>& second,
                                                                        QVector2D offset,
                                                                        QVector2D delta_intersect,
                                                                        int64_t intersect_count) {
  if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return -CalculateDistanceToObjectMayIntersect(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyCircle>(second),
        offset, delta_intersect,
        intersect_count);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return -CalculateDistanceToObjectMayIntersect(
        std::dynamic_pointer_cast<RigidBodyRectangle>(first),
        std::dynamic_pointer_cast<RigidBodyCircle>(second),
        offset, delta_intersect,
        intersect_count);
  } else if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return -CalculateDistanceToObjectMayIntersect(
        std::dynamic_pointer_cast<RigidBodyCircle>(first),
        std::dynamic_pointer_cast<RigidBodyRectangle>(second),
        offset, delta_intersect,
        intersect_count);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return -CalculateDistanceToObjectMayIntersect(
        std::dynamic_pointer_cast<RigidBodyRectangle>(first),
        std::dynamic_pointer_cast<RigidBodyRectangle>(second),
        offset, delta_intersect,
        intersect_count);
  }
  return QVector2D();
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersect(const std::shared_ptr<
    RigidBodyCircle>& circle,
                                                                  const std::shared_ptr<
                                                                      RigidBodyRectangle>& rectangle,
                                                                  QVector2D offset,
                                                                  QVector2D delta_intersect,
                                                                  int64_t intersect_count) {

  float l = 0;
  float r = 1;
  while (r - l > kEps) {
    float m = (l + r) / 2;
    if (GetIntersectPoints(circle, rectangle,
                            offset - delta_intersect * m).size() < intersect_count) {
      l = m + kEps;
    } else {
      r = m;
    }
  }
  return -delta_intersect * l;
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersect(const std::shared_ptr<
    RigidBodyRectangle>& rectangle,
                                                                  const std::shared_ptr<
                                                                      RigidBodyCircle>& circle,
                                                                  QVector2D offset,
                                                                  QVector2D delta_intersect,
                                                                  int64_t intersect_count) {
  return CalculateDistanceToObjectMayIntersect(circle, rectangle,
                                                 -offset, -delta_intersect,
                                                 intersect_count);
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersect(const std::shared_ptr<
    RigidBodyCircle>& circle1,
                                                                  const std::shared_ptr<
                                                                      RigidBodyCircle>& circle2,
                                                                  QVector2D offset,
                                                                  QVector2D delta_intersect,
                                                                  int64_t intersect_count) {
  return QVector2D();
}

QVector2D IntersectChecker::CalculateDistanceToObjectMayIntersect(const std::shared_ptr<
    RigidBodyRectangle>& rectangle1,
                                                                  const std::shared_ptr<
                                                                      RigidBodyRectangle>& rectangle2,
                                                                  QVector2D offset,
                                                                  QVector2D delta_intersect,
                                                                  int64_t intersect_count) {
  return QVector2D();
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersect(
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    const std::shared_ptr<RigidBodyCircle>& circle,
    QVector2D offset, QVector2D delta_intersect) {
  return CalculateDistanceToObjectNotToIntersect(circle, rectangle,
                                                 -offset, -delta_intersect);
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersect(
    const std::shared_ptr<RigidBodyCircle>& circle1,
    const std::shared_ptr<RigidBodyCircle>& circle2,
    QVector2D offset, QVector2D delta_intersect) {
  return QVector2D();
}

QVector2D IntersectChecker::CalculateDistanceToObjectNotToIntersect(
    const std::shared_ptr<RigidBodyRectangle>& rectangle1,
    const std::shared_ptr<RigidBodyRectangle>& rectangle2,
    QVector2D offset, QVector2D delta_intersect) {
  return QVector2D();
}

bool IntersectChecker::IsSimilarVectors(QVector2D first, QVector2D second) {
  qInfo() << first << second;
  return QVector2D::dotProduct(first, second) > 1 - kCosEps;
}
