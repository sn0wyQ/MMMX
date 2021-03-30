#include "rigid_body_circle.h"

RigidBodyCircle::RigidBodyCircle(float radius)
  : radius_(radius) {}

float RigidBodyCircle::GetRadius() const {
  return radius_;
}

std::shared_ptr<RigidBodyCircle> RigidBodyCircle::External(float friction_force)
  const {
  auto new_circle_ptr = std::make_shared<RigidBodyCircle>(*this);
  new_circle_ptr->radius_ += friction_force;
  return new_circle_ptr;
}

void RigidBodyCircle::SetRadius(float radius) {
  radius_ = radius;
}
