#include "map_border.h"

MapBorder::MapBorder(GameObjectId game_object_id)
  : GameObject(game_object_id) {}

std::shared_ptr<GameObject> MapBorder::Clone() const {
  return std::make_shared<MapBorder>(*this);
}

GameObjectType MapBorder::GetType() const {
  return GameObjectType::kMapBorder;
}

bool MapBorder::IsFilteredByFov() const {
  return false;
}
