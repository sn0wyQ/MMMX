#ifndef GAMEOBJECT_MAP_BORDER_H_
#define GAMEOBJECT_MAP_BORDER_H_

#include <memory>

#include "GameObject/game_object.h"

class MapBorder : public GameObject {
 public:
  explicit MapBorder(GameObjectId game_object_id);
  MapBorder(const MapBorder& other) = default;
  virtual ~MapBorder() = default;

  GameObjectType GetType() const override;

  bool IsFilteredByFov() const override;

  std::shared_ptr<GameObject> Clone() const override;
};

#endif  // GAMEOBJECT_MAP_BORDER_H_
