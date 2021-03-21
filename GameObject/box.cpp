#include "box.h"

Box::Box(float x, float y, int width, int height)
  : GameObject(Constants::kNullGameObjectId, QPointF(x, y),
               std::make_shared<RigidBodyRectangle>(width, height)),
  width_(width), height_(height) {
}


void Box::DrawRelatively(Painter* painter) {}

void Box::OnTick(int) {}
