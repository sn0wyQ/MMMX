#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  MovableObject(GameObjectId id, std::shared_ptr<RigidBodyCircle> rigid_body);

  void OnTick(int time_from_previous_tick) override;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  QVector2D GetAppliedDeltaPosition(int time_from_previous_tick) const;
  QVector2D GetVelocityByDeltaPosition(QVector2D position,
                                       int time_from_previous_tick) const;
  void ApplyVelocity(int time_from_previous_tick);

  float GetCurrentSpeed() const;

  std::shared_ptr<RigidBodyCircle> GetRigidBody() const;

 private:
  QVector2D velocity_{};
  std::shared_ptr<RigidBodyCircle> rigid_body_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
