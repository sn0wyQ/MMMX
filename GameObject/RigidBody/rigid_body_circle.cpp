#include "rigid_body_circle.h"

RigidBodyCircle::RigidBodyCircle(float radius)
  : radius_(radius) {}

void RigidBodyCircle::Draw(Painter* painter) const {
  painter->save();
  painter->setPen(Qt::red);
  painter->DrawEllipse(QPointF(), radius_, radius_);
  painter->restore();
}

float RigidBodyCircle::GetRadius() const {
  return radius_;
}

std::shared_ptr<RigidBodyCircle> RigidBodyCircle::External() const {
  auto new_circle_ptr = std::make_shared<RigidBodyCircle>(*this);
  new_circle_ptr->radius_ += IntersectChecker::kFrictionForce;
  return new_circle_ptr;
}
