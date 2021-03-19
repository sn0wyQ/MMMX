#include "rigid_body_circle.h"

RigidBodyCircle::RigidBodyCircle(float radius)
  : radius_(radius) {}

void RigidBodyCircle::Draw(Painter* painter) {
  StartDrawing(painter);
  painter->DrawEllipse(QPointF(), radius_, radius_);
  EndDrawing(painter);
}

RigidBodyType RigidBodyCircle::GetRigidBodyType() const {
  return RigidBodyType::kCircle;
}

float RigidBodyCircle::GetRadius() const {
  return radius_;
}
