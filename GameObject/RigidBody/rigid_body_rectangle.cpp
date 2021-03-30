#include "rigid_body_rectangle.h"

RigidBodyRectangle::RigidBodyRectangle(float width, float height)
  : width_(width), height_(height) {}

float RigidBodyRectangle::GetWidth() const {
  return width_;
}

float RigidBodyRectangle::GetHeight() const {
  return height_;
}

void RigidBodyRectangle::SetWidth(float width) {
  width_ = width;
}

void RigidBodyRectangle::SetHeight(float height) {
  height_ = height;
}
