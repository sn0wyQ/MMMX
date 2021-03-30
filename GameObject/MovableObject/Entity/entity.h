#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);

  void OnTick(int delta_time) override = 0;
  void Draw(Painter* painter) override = 0;

  float GetViewAngle() const;
  void SetViewAngle(float view_angle);

 private:
  // 0 is direction from left to right
  // Increasing counterclockwise
  // Measured in degrees
  float view_angle_{0.f};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
