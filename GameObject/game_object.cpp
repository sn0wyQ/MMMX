#include "game_object.h"

GameObject::GameObject(GameObjectId id) : id_(id) {}

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
