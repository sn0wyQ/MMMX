#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_

#include <memory>

#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

class RigidBodyCircle : public RigidBody {
 public:
  RigidBodyCircle();
  explicit RigidBodyCircle(float radius);

  void SetRadius(float radius);
  float GetRadius() const;

  RigidBodyType GetType() const override;
  std::shared_ptr<RigidBody> External(float friction_force) const override;
  void Draw(Painter* painter) override;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_CIRCLE_H_
