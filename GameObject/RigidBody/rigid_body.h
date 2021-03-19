#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_

#include <QVector2D>

#include "Painter/painter.h"

enum class RigidBodyType {
  kCircle,
  kRectangle
};

class RigidBody {
 public:
  virtual void Draw(Painter* painter) = 0;
  void StartDrawing(Painter* painter);
  void EndDrawing(Painter* painter);
  virtual RigidBodyType GetRigidBodyType() const = 0;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_H_
