#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_

#include <memory>

#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

class RigidBodyRectangle : public RigidBody {
 public:
  RigidBodyRectangle();
  RigidBodyRectangle(float width, float height);
  virtual ~RigidBodyRectangle() = default;

  RigidBodyType GetType() const override;
  std::shared_ptr<RigidBody> External(float friction_force) const override;
  void Draw(Painter* painter) override;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
