#include "box.h"

Box::Box(int width, int height)
  : GameObject(Constants::kNullGameObjectId, QPointF(-5.f, -5.f),
               std::make_shared<RigidBodyRectangle>(width, height)),
  width_(width), height_(height) {
}


void Box::DrawRelatively(Painter* painter) {}

void Box::OnTick(int) {}
