#ifndef GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
#define GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_

#include "GameObject/RigidBody/intersect_checker.h"
#include "GameObject/game_object.h"
#include "GameObject/MovableObject/movable_object.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace ObjectCollision {

  void DoSolidCollisionWithObjects(
      const std::shared_ptr<MovableObject>& main,
      const std::vector<std::shared_ptr<GameObject>>& objects,
      QVector2D force, int time_from_previous_tick);

  // Далее сюда можно добавить функции, которые будут обрабатывать столкновения
  // другого типа (с отталкиванием, с прилипанием)
}  // namespace ObjectCollision

#endif  // GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
