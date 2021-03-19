#include "box.h"

Box::Box(int width, int height)
  : GameObject(Constants::kNullGameObjectId,
               std::make_shared<RigidBodyRectangle>(width, height)),
  width_(width), height_(height) {
}


void Box::DrawRelatively(Painter* painter) {}

void Box::OnTick(int) {}
