#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_

#include <QPointF>

#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

class RigidBodyCircle : public RigidBody {
 public:
  explicit RigidBodyCircle(float radius);
  void Draw(Painter* painter) override;
  RigidBodyType GetRigidBodyType() const override;
  float GetRadius() const;
  std::shared_ptr<RigidBody> External() const override;

 private:
  float radius_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_
