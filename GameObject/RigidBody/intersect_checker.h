#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_

#include <math.h>

#include <QDebug>

#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace IntersectChecker {
  const double kEps = 1e-7;

  bool IsIntersectBodies(const std::shared_ptr<RigidBody>& first,
                   const std::shared_ptr<RigidBody>& second,
                   QPointF offset);

  bool IsPointInSegment(QPointF first, QPointF second, QPointF point);

  bool IsIntersect(const std::shared_ptr<RigidBodyCircle>& circle,
                   const std::shared_ptr<RigidBodyRectangle>& rectangle,
                   QPointF offset);

  bool IsIntersect(const std::shared_ptr<RigidBodyRectangle>& rectangle,
                   const std::shared_ptr<RigidBodyCircle>& circle,
                   QPointF offset);

  bool IsIntersect(const std::shared_ptr<RigidBodyCircle>& circle1,
                   const std::shared_ptr<RigidBodyCircle>& circle2,
                   QPointF offset);

  bool IsIntersect(const std::shared_ptr<RigidBodyRectangle>& rectangle1,
                   const std::shared_ptr<RigidBodyRectangle>& rectangle2,
                   QPointF offset);
};

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
