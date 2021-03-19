#include "rigidbody_rectangle.h"
RigidbodyRectangle::RigidbodyRectangle(int width, int height)
  : width_(width), height_(height) {}

void RigidbodyRectangle::Draw(Painter* painter) {
  StartDrawing(painter);
  painter->drawRect(- width_ / 2,
                    - height_ / 2,
                    width_, height_);
  EndDrawing(painter);
}
