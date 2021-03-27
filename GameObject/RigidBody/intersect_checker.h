#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_

#include <cmath>
#include <vector>

#include <QDebug>
#include <QVector2D>

#include "Math/math.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace IntersectChecker {
  std::vector<QPointF> GetIntersectPoints(
      const std::shared_ptr<RigidBodyCircle>& circle,
      const std::shared_ptr<RigidBodyRectangle>& rectangle,
      QVector2D offset, float rotation);

  std::vector<QPointF> GetIntersectPoints(
      const std::shared_ptr<RigidBodyCircle>& circle1,
      const std::shared_ptr<RigidBodyCircle>& circle2,
      QVector2D offset, float rotation);

  std::vector<QPointF> GetLineWithCircleIntersectPoints(
      float a, float b, float c, float r);

  bool IsPointInSegment(QPointF first, QPointF second, QPointF point);

  template <typename T>
  QVector2D CalculateDistanceToObjectNotToIntersectBodies(
      const std::shared_ptr<RigidBodyCircle>& first,
      const std::shared_ptr<T>& second,
      QVector2D offset, float rotation, QVector2D delta_intersect) {
    std::vector<QPointF> points
      = GetIntersectPoints(first, second, offset - delta_intersect, rotation);
    QPointF middle_point;
    if (points.empty()) {
      return delta_intersect;
    } else if(points.size() == 1) {
      middle_point = points[0];
    } else {
      middle_point = (points[0] + points[1]) / 2.f;
    }

    auto length = std::max(
        0.f, static_cast<float>(middle_point.x() * middle_point.x()
        + middle_point.y() * middle_point.y()) - first->GetRadius());
    return delta_intersect * length;
  }

  bool IsSimilarVectors(QVector2D first, QVector2D second);
};

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
