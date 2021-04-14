#ifndef GAME_OBJECT_INTERPOLATOR_H_
#define GAME_OBJECT_INTERPOLATOR_H_

#include <memory>

#include <QVariant>
#include <QMetaType>

#include "GameObject/game_object.h"
#include "Math/math.h"

namespace Interpolator {

void InterpolateObject(const std::shared_ptr<GameObject>& object,
                       const std::shared_ptr<GameObject>& future_object,
                       int64_t get_on_time);

QVariant GetInterpolatedParam(const QVariant& object_param,
                              const QVariant& future_object_param,
                              int64_t object_updated_time, int64_t future_object_updated_time,
                              int64_t get_on_time);

template<typename T>
T InterpolateNumber(T x1, T x3, int64_t t1, int64_t t2, int64_t t3) {
  float delta_x = x3 - x1;
  auto delta_time_full =
      static_cast<float>(t3 - t1);
  float speed_x = delta_x / delta_time_full;
  auto delta_time_first_part =
      static_cast<float>(t2 - t1);
  return static_cast<T>(x1 + speed_x * delta_time_first_part);
}

}  // namespace Interpolator

#endif  // GAME_OBJECT_INTERPOLATOR_H_
