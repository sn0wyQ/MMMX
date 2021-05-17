#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <utility>
#include <memory>
#include <vector>

#include <QVector2D>

#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  explicit MovableObject(GameObjectId game_object_id);
  MovableObject(const MovableObject& other);

  void OnTick(int delta_time) override;
  bool IsMovable() const override;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  QVector2D GetAppliedDeltaPosition(int delta_time) const;
  QVector2D GetVelocityByDeltaPosition(QVector2D position,
                                       int delta_time) const;
  void ApplyVelocity(int delta_time);

  void SetSpeedMultiplier(float speed_multiplier);
  float GetSpeedMultiplier() const;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  bool IsFilteredByFov() const override;

  float GetShortestDistance(const std::shared_ptr<GameObject>& object);

  std::shared_ptr<GameObject> Clone() const override;

  void UpdateAnimationState() override;

 private:
  QVector2D velocity_{};
  float speed_multiplier_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
