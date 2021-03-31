#include "object_collision.h"

namespace ObjectCollision {

void DoFirstPhase(
    const std::shared_ptr<MovableObject>& main,
    const std::vector<std::shared_ptr<GameObject>>& objects,
    int delta_time) {
  // Первая фаза (прижимная): если в след тик будем в объекте,
  // то делаем такую скорость, чтобы мы в след тик были прижатыми
  // к этому объекту
  for (const auto& object : objects) {
    if (main->GetId() == object->GetId()) {
      continue;
    }

    QVector2D offset = QVector2D(object->GetX() - main->GetX(),
                                 object->GetY() - main->GetY());
    float rotation = object->GetRotation();

    std::vector<QPointF> intersect_points_in_future
        = IntersectChecker::GetIntersectPointsBodies(
            main->GetRigidBody(), object->GetRigidBody(),
            offset
                - main->GetAppliedDeltaPosition(delta_time),
            rotation);

  // Если мы будем в объекте в будущем...
    if (!intersect_points_in_future.empty()) {
  // То высчитываем расстояние, чтобы мы не пересекались с объектом
  // O(log -log_10 kEps)
      QVector2D delta_to_set
          = IntersectChecker::CalculateDistanceToObjectNotToIntersectBodies(
              main->GetRigidBody(), object->GetRigidBody(),
              offset, rotation,
              main->GetAppliedDeltaPosition(delta_time));
      // С учетом текущей скорости и тика переводим расстояние в скорость
      QVector2D velocity_to_set = main->GetVelocityByDeltaPosition(
          delta_to_set, delta_time);

      // Обновляем скорость
      main->SetVelocity(velocity_to_set);
    }
  }
}

void FindTangents(const std::shared_ptr<MovableObject>& main,
                  const std::vector<std::shared_ptr<GameObject>>& objects,
                  std::vector<QVector2D>* tangents_to_push) {
  // Вторая фаза (скользкая):
  // Если наша внешняя оболочка [RigidBody::External()]
  // оказалась уже прижата,
  // то ищем все касательные к нашему объекту
  for (const auto& object : objects) {
    if (main->GetId() == object->GetId()) {
      continue;
    }

    QVector2D offset = QVector2D(object->GetX() - main->GetX(),
                                 object->GetY() - main->GetY());
    float rotation = object->GetRotation();
    std::vector<QPointF> intersect_points_now
        = IntersectChecker::GetIntersectPointsBodies(
            main->GetRigidBody()->External(object->GetFrictionForce()),
            object->GetRigidBody(),
            offset, rotation);

    for (const auto& point : intersect_points_now) {
      // Из вектора, задающего точку прикосновения
      // делаем касательную
      QVector2D tangent_vector(-point.y(), point.x());
      tangent_vector.normalize();
      tangents_to_push->push_back(tangent_vector);
    }
  }
}
void DoSecondPhase(const std::shared_ptr<MovableObject>& main,
                   const std::vector<std::shared_ptr<GameObject>>& objects,
                   QVector2D force,
                   const std::vector<QVector2D>& tangents) {
  // Если касательные нашлись, то пробуем скользить по ним
  if (!tangents.empty()) {
    bool full_stop = false;
    QVector2D common_tangent_vector;
    bool has_common = false;

    for (const auto& tangent_vector : tangents) {
      // cos = косинус угла между касательной и нажатием клавиш
      // также он будет равен будущей скорости, т.к. есть зависимость
      // чем острее угол - тем больше скалярное произведение
      float cos = QVector2D::dotProduct(
          force, QVector2D(tangent_vector.y(), -tangent_vector.x()));
      // Это проверка на то, находится ли касательная на пути нажатия клавиш
      if (cos <= 0) {
        continue;
      }
      if (!has_common) {
        has_common = true;
        common_tangent_vector = tangent_vector;
        continue;
      }

      // Если мы нашли несколько касательных, то тут два варианта
      // - они как бы замыкают нас и довольно строго, что дальше пройти
      // мы не сможем никак
      // - они более-менее похожи, тогда мы можем проскальзить по двоим сразу,
      // тогда найдем сумму, а потом ее нормализуем
      if (IntersectChecker::IsSimilarVectors(common_tangent_vector,
                                             tangent_vector)) {
        common_tangent_vector += tangent_vector;
        continue;
      }
      // Если касательные не похожи, то мы должны остановится
      full_stop = true;
      break;
    }
    if (full_stop) {
      main->SetVelocity({0.f, 0.f});
    } else {
      float length_result;
      if (has_common) {
        common_tangent_vector.normalize();
        // Теперь окончательно посчитаем скорость
        length_result = QVector2D::dotProduct(common_tangent_vector,
                                              force);
      } else {
        // Если все наши касательные нам не мешают,
        // то мы можем просто применить движение туда,
        // куда нажали клавиши
        length_result = 1.f;
        common_tangent_vector = force;
      }
      common_tangent_vector.normalize();
      QVector2D result = common_tangent_vector * length_result;
      main->SetVelocity(result);
    }
  }
}

}  // namespace ObjectCollision