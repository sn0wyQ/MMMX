#ifndef GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
#define GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_

#include <QVector2D>

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
 public:
  explicit MovableObject(GameObjectId id);
  void OnTick() override = 0;
  void Draw(Painter* painter) override = 0;

 private:
  QVector2D velocity_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_MOVABLE_OBJECT_H_
