#include "rigid_body_rectangle.h"

RigidBodyRectangle::RigidBodyRectangle(float width, float height)
  : width_(width), height_(height) {}

void RigidBodyRectangle::Draw(Painter* painter) {
  StartDrawing(painter);
  painter->DrawRect(- width_ / 2.,
                    - height_ / 2.,
                    width_, height_);
  EndDrawing(painter);
}

RigidBodyType RigidBodyRectangle::GetRigidBodyType() const {
  return RigidBodyType::kRectangle;
}

int RigidBodyRectangle::GetWidth() const {
  return width_;
}

int RigidBodyRectangle::GetHeight() const {
  return height_;
}

std::shared_ptr<RigidBody> RigidBodyRectangle::External() const {
  return std::make_shared<RigidBodyRectangle>(*this);
}
