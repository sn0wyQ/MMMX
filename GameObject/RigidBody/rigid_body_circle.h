#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_

#include <memory>

#include <QPointF>

#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "Painter/painter.h"

class RigidBodyCircle {
 public:
  explicit RigidBodyCircle(float radius);
  float GetRadius() const;
  std::shared_ptr<RigidBodyCircle> External(float friction_force) const;

 private:
  float radius_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_
