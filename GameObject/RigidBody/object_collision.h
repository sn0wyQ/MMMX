#ifndef GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
#define GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_

#include <memory>
#include <vector>

#include "GameObject/MovableObject/movable_object.h"
#include "GameObject/RigidBody/intersect_checker.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "GameObject/game_object.h"

namespace ObjectCollision {

void DoFirstPhase(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects, int delta_time);

void FindTangents(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    std::vector<QVector2D>* tangents_to_push);

void DoSecondPhase(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    QVector2D force, const std::vector<QVector2D>& tangents);

// Далее сюда можно добавить функции, которые будут обрабатывать столкновения
// другого типа (с отталкиванием, с прилипанием)

}  // namespace ObjectCollision

#endif  // GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
