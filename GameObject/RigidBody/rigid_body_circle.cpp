#include "rigid_body_circle.h"

RigidBodyCircle::RigidBodyCircle() : RigidBody() {}

RigidBodyCircle::RigidBodyCircle(float radius)
  : RigidBody(radius * 2, radius * 2) {}

std::shared_ptr<RigidBody> RigidBodyCircle::External(float friction_force)
  const {
  auto new_circle_ptr = std::make_shared<RigidBodyCircle>(*this);
  new_circle_ptr->SetRadius(GetRadius() + friction_force);
  return new_circle_ptr;
}

RigidBodyType RigidBodyCircle::GetType() const {
  return RigidBodyType::kCircle;
}

float RigidBodyCircle::GetRadius() const {
  return GetWidth() / 2.f;
}

void RigidBodyCircle::SetRadius(float radius) {
  SetWidth(radius * 2.f);
  SetHeight(radius * 2.f);
}

void RigidBodyCircle::Draw(Painter* painter) {
  painter->DrawEllipse({0.f, 0.f},
                       GetRadius(), GetRadius());
}
