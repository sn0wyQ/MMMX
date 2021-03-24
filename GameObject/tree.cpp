#include "tree.h"

Tree::Tree(GameObjectId game_object_id, float x, float y, float radius)
  : GameObject(game_object_id, QPointF(x, y), 0.f,
    std::make_shared<RigidBodyCircle>(radius)), radius_(radius) {}

void Tree::OnTick(int) {}

void Tree::DrawRelatively(Painter* painter) {
  QBrush brush(Qt::green);
  brush.setStyle( Qt::BrushStyle::CrossPattern);
  painter->setBrush(brush);
  painter->DrawEllipse(QPointF(), radius_, radius_);
}

float Tree::GetRadius() const {
  return radius_;
}

