#include "converter.h"

Converter::Converter(QWidget* widget) {
  coef_ = static_cast<float>(widget->height()) / Constants::kBaseViewHeight;
}

float Converter::GetCoef() const {
  return coef_;
}

void Converter::Update(float new_widget_height) {
  coef_ = new_widget_height / Constants::kBaseViewHeight;
}
