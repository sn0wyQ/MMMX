#include "box.h"

Box::Box(GameObjectId game_object_id, float x, float y, float rotation,
         float width, float height)
 : RectangularStaticObject(
     game_object_id, width, height,
     std::make_shared<RigidBodyRectangle>(width, height)) {
  this->SetX(x);
  this->SetY(y);
  this->SetRotation(rotation);
}

void Box::DrawRelatively(Painter* painter) {
  painter->setBrush(Qt::BrushStyle::CrossPattern);
  painter->DrawRect(-GetWidth() / 2.f, -GetHeight() / 2.f,
                    GetWidth(), GetHeight());
}

void Box::OnTick(int) {}
