#include "intersect_checker.h"

bool IntersectChecker::IsIntersect(
    const std::shared_ptr<RigidBodyCircle>& circle,
    const std::shared_ptr<RigidBodyRectangle>& rectangle,
    QPointF offset) {
  // qInfo() << offset.x() << " " << offset.y() << "\n";

  //offset *= -1;
  // double r, a, b, c; // входные данные
  double r = circle->GetRadius();
  std::vector<QPointF> points;
  points.emplace_back(offset.x() - rectangle->GetWidth() / 2.,
                           offset.y() - rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() + rectangle->GetWidth() / 2.,
                           offset.y() - rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() + rectangle->GetWidth() / 2.,
                           offset.y() + rectangle->GetHeight() / 2.);
  points.emplace_back(offset.x() - rectangle->GetWidth() / 2.,
                           offset.y() + rectangle->GetHeight() / 2.);
  for (int i = 0; i < 4; i++) {
    QPointF first = points[i];
    QPointF second = points[(i + 1) % 4];
    // qInfo() << i << first.x() << first.y() << "\n";
    double a = second.y() - first.y();
    double b = first.x() - second.x();
    double c = first.y() * second.x() - first.x() * second.y();
    // qInfo() << a << b << c;

    double x0 = -a * c / (a * a + b * b), y0 = -b * c / (a * a + b * b);
    if (c * c > r * r * (a * a + b * b) + kEps) {
      // qInfo() << "no point";
    } else if (abs(c * c - r * r * (a * a + b * b)) < kEps) {
      // qInfo() << "1 point " << x0 << ' ' << y0 << '\n';
      if (IsPointInSegment(first, second, QPointF(x0, y0))) {
        return true;
      }
    } else {
      double d = r * r - c * c / (a * a + b * b);
      double mult = std::sqrt(d / (a * a + b * b));
      double ax, ay, bx, by;
      ax = x0 + b * mult;
      bx = x0 - b * mult;
      ay = y0 - a * mult;
      by = y0 + a * mult;
      // qInfo() << ax << ' ' << ay << '\n' << bx << ' ' << by << '\n';
      if (IsPointInSegment(first, second, QPointF(ax, ay))) {
        return true;
      }
      if (IsPointInSegment(first, second, QPointF(bx, by))) {
        return true;
      }
      // qInfo() << "2 points";
    }
  }
  return false;
}

bool IntersectChecker::IsIntersect(const std::shared_ptr<RigidBodyRectangle>& rectangle,
                                   const std::shared_ptr<RigidBodyCircle>& circle,
                                   QPointF offset) {
  return IsIntersect(circle, rectangle, -offset);
}

bool IntersectChecker::IsIntersectBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QPointF offset) {
  if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return IsIntersect(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                       std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kCircle) {
    return IsIntersect(std::dynamic_pointer_cast<RigidBodyRectangle>(first),
                       std::dynamic_pointer_cast<RigidBodyCircle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kCircle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return IsIntersect(std::dynamic_pointer_cast<RigidBodyCircle>(first),
                       std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset);
  } else if (first->GetRigidBodyType() == RigidBodyType::kRectangle
      && second->GetRigidBodyType() == RigidBodyType::kRectangle) {
    return IsIntersect(std::dynamic_pointer_cast<RigidBodyRectangle>(first),
                       std::dynamic_pointer_cast<RigidBodyRectangle>(second),
                       offset);
  }
  return false;
}

bool IntersectChecker::IsIntersect(const std::shared_ptr<RigidBodyCircle>& circle1,
                                   const std::shared_ptr<RigidBodyCircle>& circle2,
                                   QPointF offset) {
  return false;
}

bool IntersectChecker::IsIntersect(const std::shared_ptr<RigidBodyRectangle>& rectangle1,
                                   const std::shared_ptr<RigidBodyRectangle>& rectangle2,
                                   QPointF offset) {
  return false;
}

bool IntersectChecker::IsPointInSegment(QPointF first,
                                        QPointF second,
                                        QPointF point) {
  double min_x = std::min(first.x(), second.x());
  double max_x = std::max(first.x(), second.x());
  double min_y = std::min(first.y(), second.y());
  double max_y = std::max(first.y(), second.y());
  // qInfo() << "mn" << min_x << max_x << min_y << max_y;
  if (min_x - kEps <= point.x() && point.x() <= max_x + kEps
    && min_y - kEps <= point.y() && point.y() <= max_y + kEps) {
    return true;
  }
  return false;
}
