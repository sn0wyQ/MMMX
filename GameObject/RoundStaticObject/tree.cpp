#include "tree.h"

Tree::Tree(GameObjectId game_object_id, float x, float y, float radius)
  : RoundStaticObject(game_object_id, radius,
                      std::make_shared<RigidBodyCircle>(radius)) {
  this->SetX(x);
  this->SetY(y);
}

void Tree::OnTick(int) {}

void Tree::DrawRelatively(Painter* painter) {
  QBrush brush(Qt::green);
  brush.setStyle( Qt::BrushStyle::CrossPattern);
  painter->setBrush(brush);
  painter->DrawEllipse(QPointF(), GetRadius(), GetRadius());
}

