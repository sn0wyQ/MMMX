#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_

#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "Painter/painter.h"

class RigidBodyRectangle {
 public:
  RigidBodyRectangle(float width, float height);

  float GetWidth() const;
  float GetHeight() const;

  void SetWidth(float width);
  void SetHeight(float height);

 private:
  float width_;
  float height_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_RECTANGLE_H_
