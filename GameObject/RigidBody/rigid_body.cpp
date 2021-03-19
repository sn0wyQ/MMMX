#include "rigid_body.h"

void RigidBody::StartDrawing(Painter* painter) {
  painter->save();
  painter->setPen(Qt::red);
}

void RigidBody::EndDrawing(Painter* painter) {
  painter->restore();
}
