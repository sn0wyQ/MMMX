#include "box.h"

Box::Box(int width, int height)
  : GameObject(Constants::kNullGameObjectId,
               std::make_shared<RigidbodyRectangle>(width, height)),
  width_(width), height_(height) {
}

void Box::Draw(Painter* painter) {

}
