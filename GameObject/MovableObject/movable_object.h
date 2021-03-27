#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <utility>
#include <memory>

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  MovableObject(GameObjectId id, std::shared_ptr<RigidBodyCircle> rigid_body);

  void OnTick(int delta_time) override;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  QVector2D GetAppliedDeltaPosition(int delta_time) const;
  QVector2D GetVelocityByDeltaPosition(QVector2D position,
                                       int delta_time) const;
  void ApplyVelocity(int delta_time);

  float GetCurrentSpeed() const;

  std::shared_ptr<RigidBodyCircle> GetRigidBody() const;

 private:
  QVector2D velocity_{};
  std::shared_ptr<RigidBodyCircle> rigid_body_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
