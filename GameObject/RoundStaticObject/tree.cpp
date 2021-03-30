#include "tree.h"

Tree::Tree(GameObjectId game_object_id, float x, float y, float radius)
  : RoundStaticObject(game_object_id, radius,
                      std::make_shared<RigidBodyCircle>(radius)) {
  this->SetX(x);
  this->SetY(y);
}

Tree::Tree(GameObjectId game_object_id, const std::vector<QVariant>& params)
    : Tree(game_object_id, params.at(0).toFloat(),
           params.at(1).toFloat(), params.at(2).toFloat()){}

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

GameObjectType Tree::GetGameObjectType() const {
  return GameObjectType::kTree;
}
