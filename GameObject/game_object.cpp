#include "game_object.h"

GameObject::GameObject(GameObjectId id) : id_(id) {}

GameObjectId GameObject::GetId() {
  return id_;
}

void GameObject::SetId(GameObjectId id) {
  id_ = id;
}
