#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);

  void OnTick(int time_from_previous_tick) override = 0;
  void Draw(Painter* painter) override = 0;

  float GetViewAngle() const;
  void SetViewAngle(float view_angle);

  bool IsInFov() const;
  void SetIsInFov(bool is_in_fov);

 private:
  // 0 is direction from left to right
  // Increasing counterclockwise
  // Measured in degrees
  float view_angle_ = 0.f;
  bool is_in_fov_{true};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
