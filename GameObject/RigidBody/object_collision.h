#ifndef GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
#define GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_

#include <memory>
#include <vector>

#include "GameObject/MovableObject/Bullet/bullet.h"
#include "GameObject/MovableObject/movable_object.h"
#include "GameObject/RigidBody/intersect_checker.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "GameObject/game_object.h"

namespace ObjectCollision {

void MoveWithSlidingCollision(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    QVector2D force, int delta_time);

void DoPressurePhase(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects, int delta_time);

std::vector<QVector2D> GetTangents(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects);

void MoveAlongTangents(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    QVector2D force, const std::vector<QVector2D>& tangents);

std::shared_ptr<GameObject> GetObjectBulletCollidedWith(
    const std::shared_ptr<Bullet>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    int delta_time, bool on_local);

bool AreCollided(
    const std::shared_ptr<GameObject>& object1,
    const std::shared_ptr<GameObject>& object2);

}  // namespace ObjectCollision

#endif  // GAMEOBJECT_RIGIDBODY_OBJECT_COLLISION_H_
