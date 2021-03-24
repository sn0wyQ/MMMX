#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_

#include <cmath>
#include <vector>

#include <QDebug>
#include <QVector3D>

#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace IntersectChecker {
  const float kEps = 1e-7f;
  const float kCosEps = 0.4f;

  std::vector<QPointF> GetIntersectPointsBodies(const std::shared_ptr<RigidBody>& first,
                                const std::shared_ptr<RigidBody>& second,
                                QVector2D offset, float rotation);

  bool IsPointInSegment(QPointF first, QPointF second, QPointF point);

  std::vector<QPointF> GetIntersectPoints(const std::shared_ptr<RigidBodyCircle>& circle,
                   const std::shared_ptr<RigidBodyRectangle>& rectangle,
                   QVector2D offset, float rotation);

  std::vector<QPointF> GetIntersectPoints(const std::shared_ptr<RigidBodyCircle>& circle1,
                   const std::shared_ptr<RigidBodyCircle>& circle2,
                   QVector2D offset, float rotation);

  // Эта функция дихает вектор изменения позиции, чтобы за счет него изменить
  // вектор velocity. Это позволит нам в случае столкновения соприкасать
  // челиков сугобо к стенке, а не оставлять некоторое расстояние
  QVector2D CalculateDistanceToObjectNotToIntersectBodies(
      const std::shared_ptr<RigidBody>& first,
      const std::shared_ptr<RigidBody>& second,
      QVector2D offset, float rotation, QVector2D delta_intersect);

  QVector2D CalculateDistanceToObjectNotToIntersect(
      const std::shared_ptr<RigidBodyCircle>& circle,
      const std::shared_ptr<RigidBodyRectangle>& rectangle,
      QVector2D offset, float rotation, QVector2D delta_intersect);

  QVector2D CalculateDistanceToObjectNotToIntersect(
      const std::shared_ptr<RigidBodyCircle>& circle1,
      const std::shared_ptr<RigidBodyCircle>& circle2,
      QVector2D offset, float rotation, QVector2D delta_intersect);

  QVector2D CalculateDistanceToObjectMayIntersectBodies(
      const std::shared_ptr<RigidBody>& first,
      const std::shared_ptr<RigidBody>& second,
      QVector2D offset, float rotation, QVector2D delta_intersect);
  
  QVector2D CalculateDistanceToObjectMayIntersect(
      const std::shared_ptr<RigidBodyCircle>& circle,
      const std::shared_ptr<RigidBodyRectangle>& rectangle,
      QVector2D offset, float rotation, QVector2D delta_intersect);
  
  QVector2D CalculateDistanceToObjectMayIntersect(
      const std::shared_ptr<RigidBodyCircle>& circle1,
      const std::shared_ptr<RigidBodyCircle>& circle2,
      QVector2D offset, float rotation, QVector2D delta_intersect);

  bool IsSimilarVectors(QVector2D first, QVector2D second);

  QVector2D GetRotatedVector(QVector2D first, QVector2D second,
                             float alpha);
};

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CHECKER_H_
