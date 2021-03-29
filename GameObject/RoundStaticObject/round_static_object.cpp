#include "round_static_object.h"

RoundStaticObject::RoundStaticObject(
    GameObjectId game_object_id, float radius,
    std::shared_ptr<RigidBodyCircle> rigid_body)
    : GameObject(game_object_id), radius_(radius),
    rigid_body_(std::move(rigid_body)) {}

std::shared_ptr<RigidBodyCircle> RoundStaticObject::GetRigidBody() const {
  return rigid_body_;
}

float RoundStaticObject::GetRadius() const {
  return radius_;
}

float RoundStaticObject::GetFrictionForce() const {
  return IntersectChecker::kFrictionForceRoundStaticObject;
}
