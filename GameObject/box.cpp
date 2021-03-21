#include "box.h"

Box::Box(float x, float y, float width, float height)
  : GameObject(Constants::kNullGameObjectId, QPointF(x, y),
               std::make_shared<RigidBodyRectangle>(width, height)),
  width_(width), height_(height) {
}


void Box::DrawRelatively(Painter* painter) {
  painter->setBrush(Qt::BrushStyle::CrossPattern);
  painter->DrawRect(-width_ / 2, -height_ / 2, width_, height_);
}

void Box::OnTick(int) {}
