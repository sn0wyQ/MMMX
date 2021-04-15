#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include <memory>
#include <utility>
#include <vector>

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);
  Entity(const Entity& other);

  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  std::shared_ptr<GameObject> Clone() const override;

 private:
  float fov_radius_{Constants::kDefaultEntityFov};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
