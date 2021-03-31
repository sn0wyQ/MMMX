#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_

#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>

#include <QVector2D>
#include <QDebug>

#include "Math/math.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace IntersectChecker {
  std::vector<QPointF> GetIntersectPointsBodies(
    const std::shared_ptr<RigidBody>& first,
    const std::shared_ptr<RigidBody>& second,
    QVector2D offset, float rotation);

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

  QVector2D CalculateDistanceToObjectNotToIntersectBodies(
      const std::shared_ptr<RigidBody>& first,
      const std::shared_ptr<RigidBody>& second,
      QVector2D offset, float rotation, QVector2D delta_intersect);

  bool IsSimilarVectors(QVector2D first, QVector2D second);
}  // namespace IntersectChecker

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
