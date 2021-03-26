#ifndef GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_
#define GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_

namespace IntersectChecker {
  // Точность для определения пересечений
  const float kEps = 1e-4f;
  // Насколько похожие касательные считаем непротивоборствующими
  // [0, 1]
  // 0 - все касательные противоборствующие
  // 1 - пытаемся скользить по всем
  const float kCosEps = 0.5f;
  // Скорость скольжение
  // (= увеличение внешней оболочки объекта при считании касательной)
  const float kSlidingSpeed = 0.1f;
}

#endif  // GAMEOBJECT_RIGIDBODY_INTERSECT_CONSTANTS_H_
