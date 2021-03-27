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

std::shared_ptr<RigidBody> RigidBodyCircle::External() const {
  auto new_circle_ptr = std::make_shared<RigidBodyCircle>(*this);
  new_circle_ptr->radius_ += IntersectChecker::kFrictionForce;
  return new_circle_ptr;
}
