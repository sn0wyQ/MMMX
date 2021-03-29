#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  explicit MovableObject(GameObjectId id);

  void OnTick(int delta_time) override = 0;
  void Draw(Painter* painter) override = 0;

  QVector2D GetVelocity() const;
  void SetVelocity(const QVector2D& velocity);
  void ApplyVelocity(int delta_time);

  float GetCurrentSpeed() const;

 private:
  QVector2D velocity_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
