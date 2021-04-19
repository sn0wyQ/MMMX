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

  void DrawHealthBar(Painter* painter) override;

  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetHealthPoints(float health_points);
  float GetHealthPoints() const;

  void SetMaxHealthPoints(float max_health_points);
  float GetMaxHealthPoints() const;

  bool IsEntity() const override;

  void Revive();

  void SetSpawnPosition(QPointF spawn_position);
  QPointF GetSpawnPosition() const;

  std::shared_ptr<GameObject> Clone() const override;

 private:
  float fov_radius_{Constants::kDefaultEntityFov};
  float health_points_{};
  float max_health_points_{};
  QPointF spawn_position_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
