#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);
  void OnTick(int time_from_previous_tick) override = 0;
  void Draw(Painter* painter) override = 0;

  float GetCurrentSpeed() const;

  float GetViewAngle() const;
  void SetViewAngle(float view_angle);

 private:
  float current_hp_ = 0.f;
  float base_max_hp_ = 0.f;
  float acceleration_ = 0.f;
  float max_speed_ = 0.f;

  // 0 is direction from left to right
  // Increasing counterclockwise
  // Measured in degrees
  float view_angle_ = 0.f;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
