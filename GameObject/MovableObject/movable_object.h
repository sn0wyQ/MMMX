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

  void OnTick(int delta_time) override;
  bool IsMovable() const override;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  QVector2D GetAppliedDeltaPosition(int delta_time) const;
  QVector2D GetVelocityByDeltaPosition(QVector2D position,
                                       int delta_time) const;
  void ApplyVelocity(int delta_time);

  float GetCurrentSpeed() const;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  bool IsFilteredByFov() const override;

  static float GetShortestDistance(
      const std::shared_ptr<MovableObject>& main,
      const std::shared_ptr<GameObject>& object);

 private:
  QVector2D velocity_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
