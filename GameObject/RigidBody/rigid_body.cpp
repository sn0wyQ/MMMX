#include "rigid_body.h"

RigidBody::RigidBody(float width, float height)
  : width_(width), height_(height) {}

float RigidBody::GetWidth() const {
  return width_;
}

float RigidBody::GetHeight() const {
  return height_;
}

void RigidBody::SetHeight(float height) {
  height_ = height;
}

void RigidBody::SetWidth(float width) {
  width_ = width;
}
