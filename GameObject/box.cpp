#include "box.h"

Box::Box(GameObjectId game_object_id, float x, float y, float rotation,
         float width, float height)
         : GameObject(game_object_id, x, y, rotation, width, height,
             std::make_shared<RigidBodyRectangle>(width, height)) {}

Box::Box(GameObjectId game_object_id, const std::vector<QVariant>& params)
  : Box(game_object_id,
        params.at(0).toFloat(),
        params.at(1).toFloat(),
        params.at(2).toFloat(),
        params.at(3).toFloat(),
        params.at(4).toFloat()) {}

void Box::DrawRelatively(Painter* painter) {
  painter->setBrush(Qt::BrushStyle::CrossPattern);
  painter->DrawRect(-GetWidth() / 2.f, -GetHeight() / 2.f,
                    GetWidth(), GetHeight());
}

void Box::OnTick(int) {}

void Box::SetParams(const std::vector<QVariant>& params) {
  SetX(params.at(0).toFloat());
  SetY(params.at(1).toFloat());
  SetRotation(params.at(2).toFloat());
  SetWidth(params.at(3).toFloat());
  SetHeight(params.at(4).toFloat());
}

std::vector<QVariant> Box::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetRotation());
  result.emplace_back(GetWidth());
  result.emplace_back(GetHeight());
  return result;
}

GameObjectType Box::GetType() const {
  return GameObjectType::kBox;
}
