#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_

#include <memory>

#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

class RigidBodyRectangle : public RigidBody {
 public:
  explicit RigidBodyRectangle(float width, float height);

  RigidBodyType GetType() const override;
  std::shared_ptr<RigidBody> External(float friction_force) const override;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
