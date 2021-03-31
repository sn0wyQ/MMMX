#include "game_object.h"

GameObject::GameObject(GameObjectId id, std::shared_ptr<RigidBody> rigid_body)
  : id_(id), rigid_body_(std::move(rigid_body)) {}

GameObject::GameObject(
    GameObjectId id, float x, float y, float rotation,
    float width, float height,
    std::shared_ptr<RigidBody> rigid_body)
    : GameObject(id, std::move(rigid_body)) {
  SetWidth(width);
  SetHeight(height);
  SetX(x);
  SetY(y);
  SetRotation(rotation);
}

GameObjectId GameObject::GetId() const {
  return id_;
}

void GameObject::SetId(GameObjectId id) {
  id_ = id;
}

QPointF GameObject::GetPosition() const {
  return position_;
}

void GameObject::SetPosition(QPointF point) {
  position_ = point;
}

void GameObject::SetX(float x) {
  position_.setX(x);
}

void GameObject::SetY(float y) {
  position_.setY(y);
}

float GameObject::GetX() const {
  return position_.x();
}

float GameObject::GetY() const {
  return position_.y();
}

void GameObject::Draw(Painter* painter) {
  painter->save();
  painter->Translate(GetPosition());
  painter->RotateCounterClockWise(rotation_);
  this->DrawRelatively(painter);
  painter->restore();
}

float GameObject::GetRotation() const {
  return rotation_;
}

void GameObject::SetRotation(float rotation) {
  rotation_ = rotation;
}

std::shared_ptr<RigidBody> GameObject::GetRigidBody() const {
  return rigid_body_;
}

float GameObject::GetFrictionForce() const {
  return IntersectChecker::kDefaultFrictionForce;
}

float GameObject::GetWidth() const {
  return width_;
}

float GameObject::GetHeight() const {
  return height_;
}

void GameObject::SetWidth(float width) {
  width_ = width;
}

void GameObject::SetHeight(float height) {
  height_ = height;
}

bool GameObject::IsMovable() const {
  return false;
}

bool GameObject::IsInFov() const {
  return is_in_fov_;
}

void GameObject::SetIsInFov(bool is_in_fov) {
  is_in_fov_ = is_in_fov;
}
