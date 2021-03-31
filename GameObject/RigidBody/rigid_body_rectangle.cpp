#include "rigid_body_rectangle.h"

RigidBodyRectangle::RigidBodyRectangle() : RigidBody() {}

RigidBodyRectangle::RigidBodyRectangle(float width, float height)
  : RigidBody(width, height) {}

RigidBodyType RigidBodyRectangle::GetType() const {
  return RigidBodyType::kRectangle;
}

std::shared_ptr<RigidBody> RigidBodyRectangle::External(float friction_force)
  const {
  auto new_rect_ptr = std::make_shared<RigidBodyRectangle>(*this);
  new_rect_ptr->SetWidth(this->GetWidth() + friction_force);
  new_rect_ptr->SetHeight(this->GetHeight() + friction_force);
  return new_rect_ptr;
}
void RigidBodyRectangle::Draw(Painter* painter) {
  painter->DrawRect(- GetWidth() / 2.f, - GetHeight() / 2.f,
                    GetWidth(), GetHeight());
}
