#include "GameObject/interpolator.h"

namespace Interpolator {

void InterpolateObject(const std::shared_ptr<GameObject>& object,
                       const std::shared_ptr<GameObject>& future_object,
                       int64_t get_on_time) {
  auto object_updated_time = object->GetUpdatedTime();
  auto future_object_updated_time = future_object->GetUpdatedTime();

  float object_rotation = object->GetRotation();
  float future_object_rotation = future_object->GetRotation();
  bool change_rotation_axis = false;
  if (std::abs(future_object_rotation - object_rotation) > 180.f) {
    change_rotation_axis = true;
    object->SetRotation(Math::GetRotationInOtherAxis(object_rotation));
    future_object->SetRotation(
        Math::GetRotationInOtherAxis(future_object_rotation));
  }

  auto object_params = object->GetParams();
  auto future_object_params = future_object->GetParams();
  if (object_params.size() != future_object_params.size()) {
    throw std::runtime_error("Interpolation: size of params differs. Maybe "
                             "you send game objects with different classes?");
  }

  for (size_t i = 0; i < object_params.size(); i++) {
    auto object_param = object_params.at(i);
    auto future_object_param = future_object_params.at(i);
    object_params[i] = GetInterpolatedParam(
        object_param, future_object_param,
        object_updated_time, future_object_updated_time,
        get_on_time);
  }
  object->SetParams(object_params);

  if (change_rotation_axis) {
    object->SetRotation(Math::GetRotationInOtherAxis(object->GetRotation()));
    future_object->SetRotation(
        Math::GetRotationInOtherAxis(future_object->GetRotation()));
  }

  object->SetUpdatedTime(get_on_time);
}

QVariant GetInterpolatedParam(const QVariant& object_param,
                              const QVariant& future_object_param,
                              int64_t object_updated_time,
                              int64_t future_object_updated_time,
                              int64_t get_on_time) {
  QVariant result;
  if (object_updated_time == future_object_updated_time) {
    return future_object_param;
  }
  if (object_param.type() != future_object_param.type()) {
    throw std::runtime_error("Interpolation: type of param differs. Maybe "
                             "you send game objects with different classes?");
  }
  // QVariant::type() returns the storage type of the value stored
  // in the variant.
  // Although this function is declared as returning QVariant::Type,
  // the return value should be interpreted as QMetaType::Type.
  QMetaType::Type type = static_cast<QMetaType::Type>(object_param.type());
  switch (type) {
    case QMetaType::Type::Float: {
      float x1 = object_param.toFloat();
      float x3 = future_object_param.toFloat();
      float delta_x = x3 - x1;
      auto delta_time_full =
          static_cast<float>(future_object_updated_time - object_updated_time);
      float speed_x = delta_x / delta_time_full;
      auto delta_time_first_part =
          static_cast<float>(get_on_time - object_updated_time);
      float x_2 = x1 + speed_x * delta_time_first_part;
      result = x_2;
      break;
    }
    case QMetaType::Type::Int:
    case QMetaType::Type::LongLong: {
      int64_t x1 = object_param.toLongLong();
      int64_t x3 = future_object_param.toLongLong();
      float delta_x = x3 - x1;
      auto delta_time_full =
          static_cast<float>(future_object_updated_time - object_updated_time);
      float speed_x = delta_x / delta_time_full;
      auto delta_time_first_part =
          static_cast<float>(get_on_time - object_updated_time);
      float x_2 = x1 + speed_x * delta_time_first_part;
      result = int64_t(x_2);
      break;
    }
    case QMetaType::Type::Bool: {
      if (object_param.toBool() != future_object_param.toBool()) {
        throw std::runtime_error("Interpolation: Can't interpolate "
                                 "two different bools");
      }
      break;
    }
    case QMetaType::Type::QPointF:
    case QMetaType::Type::QVector2D: {
      QPointF first_value = object_param.toPointF();
      QPointF second_value = object_param.toPointF();
      auto first_x = static_cast<float>(first_value.x());
      auto second_x = static_cast<float>(second_value.x());
      auto first_y = static_cast<float>(first_value.y());
      auto second_y = static_cast<float>(second_value.y());
      result = QPointF(
          GetInterpolatedParam(
              first_x, second_x, object_updated_time, future_object_updated_time,
              get_on_time).toFloat(),
          GetInterpolatedParam(
              first_y, second_y, object_updated_time, future_object_updated_time,
              get_on_time).toFloat());
      break;
    }
    default: {
      std::string type_name(object_param.typeName());
      throw std::runtime_error("Interpolation: Can't interpolate. "
                               "Unknown type: " + type_name);
    }
  }
  return result;
}

}  // namespace Interpolator