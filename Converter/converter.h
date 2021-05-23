#ifndef CONVERTER_CONVERTER_H_
#define CONVERTER_CONVERTER_H_

#include <cmath>

#include <QPointF>
#include <QWidget>

#include "Constants/constants.h"

class Converter {
 public:
  explicit Converter(QWidget* widget);

  QPointF GetGameCenter() const;
  float GetCoefficient() const;

  void UpdateGameCenter(QPointF center);

  void UpdateCoefficient(float fov = Constants::kDefaultEntityFov);

  QPointF PointFromGameToScreen(const QPointF& point) const;
  QPointF PointFromScreenToGame(const QPointF& point) const;

  float ScaleFromGameToScreen(float x) const {
    return std::round(x * coefficient_);
  }
  template<typename T>
  T ScaleFromGameToScreen(const T& object) const {
    return object * coefficient_;
  }
  template<typename T>
  T ScaleFromGameToScreen(T* object) const {
    *object = ScaleFromGameToScreen(*object);
    return *object;
  }

  template<typename T>
  T ScaleFromScreenToGame(const T& object) const {
    return object / coefficient_;
  }
  template<typename T>
  T ScaleFromScreenToGame(T* object) const {
    return (*object) /= coefficient_;
  }

 private:
  float coefficient_;
  QWidget* widget_;
  QPointF game_center_;
  QPointF screen_center_;
};

#endif  // CONVERTER_CONVERTER_H_
