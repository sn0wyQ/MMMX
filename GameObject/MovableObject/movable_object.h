#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  explicit MovableObject(GameObjectId id);

  void OnTick(int time_from_previous_tick) override = 0;
  void Draw(Painter* painter) override = 0;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  void ApplyVelocity(int time_from_previous_tick);

  float GetCurrentSpeed() const;

 private:
  QVector2D velocity_{};
  float acceleration_ = 0.f;
  float max_speed_ = 0.f;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
