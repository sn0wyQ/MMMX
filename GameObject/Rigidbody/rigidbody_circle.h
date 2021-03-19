#ifndef GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_
#define GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_

#include <QPointF>

#include "GameObject/Rigidbody/rigidbody.h"
#include "Painter/painter.h"

class RigidbodyCircle : public Rigidbody {
 public:
  explicit RigidbodyCircle(float radius);
  void Draw(Painter* painter) override;

 private:
  float radius_;
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGIDBODY_CIRCLE_H_
