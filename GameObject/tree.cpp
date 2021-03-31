#include "tree.h"

Tree::Tree(GameObjectId game_object_id, float x, float y, float radius)
  : GameObject(game_object_id, x, y,
               0.f, radius * 2.f, radius * 2.f,
                      std::make_shared<RigidBodyCircle>(radius)) {
}

Tree::Tree(GameObjectId game_object_id, const std::vector<QVariant>& params)
    : Tree(game_object_id, params.at(0).toFloat(),
           params.at(1).toFloat(), params.at(2).toFloat()) {}

void Tree::OnTick(int) {}

void Tree::DrawRelatively(Painter* painter) {
  QBrush brush(Qt::green);
  brush.setStyle(Qt::BrushStyle::CrossPattern);
  painter->setBrush(brush);
  painter->DrawEllipse(QPointF(), GetRadius(), GetRadius());
}

void Tree::SetParams(const std::vector<QVariant>& params) {
  SetX(params.at(0).toFloat());
  SetY(params.at(1).toFloat());
  SetRadius(params.at(2).toFloat());
}

std::vector<QVariant> Tree::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetRadius());
  return result;
}

GameObjectType Tree::GetType() const {
  return GameObjectType::kTree;
}

void Tree::SetRadius(float radius) {
  SetWidth(radius * 2.f);
  SetHeight(radius * 2.f);
}

float Tree::GetRadius() const {
  return GetWidth() / 2.f;
}
