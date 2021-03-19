#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_

#include "GameObject/Rigidbody/rigidbody.h"
#include "Painter/painter.h"

class RigidbodyRectangle : public Rigidbody {
 public:
  RigidbodyRectangle(int width, int height);
  void Draw(Painter* painter) override;

 private:
  int width_;
  int height_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_RECTANGLE_H_
