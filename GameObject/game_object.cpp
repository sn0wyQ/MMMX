#include "game_object.h"

#include <utility>

//     rigidbody_(std::make_shared<RigidbodyRectangle>(rigidbody_rectangle)) {}

// GameObject::GameObject(GameObjectId id, const RigidbodyCircle& rigidbody_circle)
//   : id_(id), rigidbody_(std::make_shared<RigidbodyCircle>(rigidbody_circle)) {}
//
// GameObject::GameObject(GameObjectId id,
//                        const RigidbodyRectangle& rigidbody_rectangle)
//     : id_(id),
GameObject::GameObject(GameObjectId id, std::shared_ptr<Rigidbody> rigidbody)
  : id_(id), rigidbody_(std::move(rigidbody)) {}

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
  if (Constants::kShowRigidbody) {
    rigidbody_->Draw(painter);
  }
}
