#ifndef CONVERTER_CONVERTER_H_
#define CONVERTER_CONVERTER_H_

#include <QPoint>
#include <QWidget>

#include "constants.h"

class Converter {
 public:
  explicit Converter(QWidget* widget);

  float GetCoef() const;

  void Update(float new_widget_height);

  template<typename T>
  T FromGameToScreen(const T& object) {
    return object * coef_;
  }
  template<typename T>
  T FromScreenToGame(const T& object) {
    return object / coef_;
  }

 private:
  float coef_;
};

#endif  // CONVERTER_CONVERTER_H_
