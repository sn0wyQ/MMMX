#ifndef CONVERTER_CONVERTER_H_
#define CONVERTER_CONVERTER_H_

#include <QPointF>
#include <QWidget>

#include "constants.h"

class Converter {
 public:
  explicit Converter(QWidget* widget);

  QPointF GetCenter() const;
  float GetCoef() const;

  void UpdateGameCenter(QPointF center);

  void UpdateCoefficient();

  QPointF PointFromGameToScreen(const QPointF& point);
  QPointF PointFromScreenToGame(const QPointF& point);

  template<typename T>
  T ScaleFromGameToScreen(const T& object) {
    return object * coefficient_;
  }
  template<typename T>
  T ScaleFromScreenToGame(const T& object) {
    return object / coefficient_;
  }

 private:
  float coefficient_;
  QWidget* widget_;
  QPointF game_center_;
  QPointF screen_center_;
};

#endif  // CONVERTER_CONVERTER_H_
