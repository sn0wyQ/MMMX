#include "rigidbody.h"

void Rigidbody::StartDrawing(Painter* painter) {
  painter->save();
  painter->setPen(Qt::red);
}

void Rigidbody::EndDrawing(Painter* painter) {
  painter->restore();
}
