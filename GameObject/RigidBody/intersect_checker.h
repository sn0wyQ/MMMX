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
    float l = 0;
    float r = 1;
    while (r - l > kEps) {
      float m = (l + r) / 2;
      if (!GetIntersectPoints(first, second,
                              offset - delta_intersect * m, rotation).empty()) {
        r = m - kEps;
      } else {
        l = m;
      }
    }
    return delta_intersect * l;
  }

  bool IsSimilarVectors(QVector2D first, QVector2D second);
};

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
