#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_

namespace IntersectChecker {
  // Точность для определения пересечений
  constexpr float kEps = 1e-4f;
  // Насколько похожие касательные считаем непротивоборствующими
  // [0, 1]
  // 0 - все касательные противоборствующие
  // 1 - пытаемся скользить по всем
  constexpr float kCosEps = 0.1f;
  // Сила трения
  // (= увеличение внешней оболочки объекта при считании касательной)
  constexpr float kDefaultFrictionForce = 0.02f;
}  // namespace IntersectChecker

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_
