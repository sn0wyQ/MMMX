#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include <memory>
#include <utility>
#include <vector>

#include <QPainterPath>

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);
  Entity(const Entity& other);
  virtual ~Entity() = default;

  void OnTick(int delta_time) override;
  void TickHealthPoints(int delta_time);

  void DrawHealthBar(Painter* painter) override;
  void DrawLevel(Painter* painter) override;

  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetHealthPoints(float health_points);
  float GetHealthPoints() const;

  void SetMaxHealthPoints(float max_health_points);
  float GetMaxHealthPoints() const;
  bool IsAlive() const override;

  void SetLevel(int level);
  int GetLevel() const;

  bool IsEntity() const override;

  void Revive(QPointF point_to_spawn);

  void SetHealthRegenRate(float health_regen_rate);
  float GetHealthRegenRate() const;

  virtual float GetExpIncrementForKill() const;

  std::shared_ptr<GameObject> Clone() const override;

 protected:
  int level_{1};

 private:
  float fov_radius_{Constants::kDefaultEntityFov};
  float health_points_{};
  float health_regen_rate_{};
  float max_health_points_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
