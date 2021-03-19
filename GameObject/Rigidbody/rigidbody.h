#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_

#include "Painter/painter.h"

class Rigidbody {
 public:
  virtual void Draw(Painter* painter) = 0;

  void StartDrawing(Painter* painter);
  void EndDrawing(Painter* painter);
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_
