#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_

#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "Painter/painter.h"

class RigidBodyRectangle {
 public:
  RigidBodyRectangle(float width, float height);
  void Draw(Painter* painter) const;

  float GetWidth() const;
  float GetHeight() const;

 private:
  float width_;
  float height_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
