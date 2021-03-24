#include "box.h"

Box::Box(GameObjectId game_object_id,
         float x,
         float y,
         float width,
         float height)
         : GameObject(
             game_object_id,
             QPointF(x, y),
             std::make_shared<RigidBodyRectangle>(width, height)),
             width_(width), height_(height) {}

void Box::DrawRelatively(Painter* painter) {
  painter->setBrush(Qt::BrushStyle::CrossPattern);
  painter->DrawRect(-width_ / 2, -height_ / 2, width_, height_);
}

void Box::OnTick(int) {}

float Box::GetWidth() const {
  return width_;
}

float Box::GetHeight() const {
  return height_;
}
