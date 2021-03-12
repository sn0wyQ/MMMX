#include "game_object.h"

GameObject::GameObject(GameObjectId id) : id_(id) {}

GameObjectId GameObject::GetId() {
  return id_;
}

void GameObject::SetId(GameObjectId id) {
  id_ = id;
}

QPoint GameObject::GetPosition() const {
  return position_;
}

void GameObject::SetPosition(QPoint point) {
  position_ = point;
}

void GameObject::SetX(int x) {
  position_.setX(x);
}

void GameObject::SetY(int y) {
  position_.setY(y);
}

int GameObject::GetX() const {
  return position_.x();
}

int GameObject::GetY() const {
  return position_.y();
}
