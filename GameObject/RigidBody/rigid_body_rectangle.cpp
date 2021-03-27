#include "rigid_body_rectangle.h"

RigidBodyRectangle::RigidBodyRectangle(float width, float height)
  : width_(width), height_(height) {}

void RigidBodyRectangle::Draw(Painter* painter) const {
  painter->save();
  painter->setPen(Qt::red);
  painter->DrawRect(- width_ / 2.f,
                    - height_ / 2.f,
                    width_, height_);
  painter->restore();
}

float RigidBodyRectangle::GetWidth() const {
  return width_;
}

float RigidBodyRectangle::GetHeight() const {
  return height_;
}
