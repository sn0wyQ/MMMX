#include "game_object.h"

GameObject::GameObject(GameObjectId game_object_id)
  : id_(game_object_id) {}

GameObjectId GameObject::GetId() const {
  return id_;
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
  if (Constants::kRigidBodyShow) {
    painter->setPen(Qt::red);
    rigid_body_->Draw(painter);
  }
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

void GameObject::SetParams(std::vector<QVariant> params) {
  auto rigid_body_type = static_cast<RigidBodyType>(params.back().toInt());
  if (rigid_body_ == nullptr) {
    switch (rigid_body_type) {
      case RigidBodyType::kCircle:
        rigid_body_ = std::make_shared<RigidBodyCircle>();
        break;
      case RigidBodyType::kRectangle:
        rigid_body_ = std::make_shared<RigidBodyRectangle>();
        break;
    }
  }
  params.pop_back();
  SetHeight(params.back().toFloat());
  params.pop_back();
  SetWidth(params.back().toFloat());
  params.pop_back();
  SetRotation(params.back().toFloat());
  params.pop_back();
  SetY(params.back().toFloat());
  params.pop_back();
  SetX(params.back().toFloat());
  params.pop_back();
  rigid_body_->SetWidth(GetWidth());
  rigid_body_->SetHeight(GetHeight());
}

std::vector<QVariant> GameObject::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetRotation());
  result.emplace_back(GetWidth());
  result.emplace_back(GetHeight());
  result.emplace_back(static_cast<int>(rigid_body_->GetType()));
  return result;
}

GameObjectType GameObject::GetType() const {
  return GameObjectType::kGameObject;
}
