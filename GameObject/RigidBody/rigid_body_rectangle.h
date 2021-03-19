#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_

#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

class RigidBodyRectangle : public RigidBody {
 public:
  RigidBodyRectangle(float width, float height);
  void Draw(Painter* painter) override;
  RigidBodyType GetRigidBodyType() const override;

  int GetWidth() const;
  int GetHeight() const;

 private:
  float width_;
  float height_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
