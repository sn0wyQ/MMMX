#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  MovableObject(GameObjectId id, std::shared_ptr<Rigidbody> rigidbody);

  void OnTick(int time_from_previous_tick) override = 0;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  void ApplyVelocity(int time_from_previous_tick);

  float GetCurrentSpeed() const;

 private:
  QVector2D velocity_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
