#include "rigidbody_circle.h"

RigidbodyCircle::RigidbodyCircle(float radius)
  : radius_(radius) {}

void RigidbodyCircle::Draw(Painter* painter) {
  StartDrawing(painter);
  painter->DrawEllipse(QPointF(), radius_, radius_);
  EndDrawing(painter);
}
