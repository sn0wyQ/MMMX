#include "rectangular_static_object.h"

RectangularStaticObject::RectangularStaticObject(
    GameObjectId game_object_id, float width, float height,
    std::shared_ptr<RigidBodyRectangle> rigid_body)
    : GameObject(game_object_id), width_(width), height_(height),
    rigid_body_(std::move(rigid_body)) {}

std::shared_ptr<RigidBodyRectangle>
    RectangularStaticObject::GetRigidBody() const {
  return rigid_body_;
}

float RectangularStaticObject::GetWidth() const {
  return width_;
}

float RectangularStaticObject::GetHeight() const {
  return height_;
}

float RectangularStaticObject::GetFrictionForce() const {
  return IntersectChecker::kFrictionForceRectangularStaticObject;
}
