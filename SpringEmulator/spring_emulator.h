#ifndef SPRINGEMULATOR_SPRING_EMULATOR_H_
#define SPRINGEMULATOR_SPRING_EMULATOR_H_

#include <QVector2D>

template <class T>
class SpringEmulator {
 public:
  SpringEmulator(float stiffness_ratio, float friction_ratio_);

  void MakeStep(const T& new_value);

  void SetValue(const T& last_value);
  T GetValue() const;

 private:
  float stiffness_ratio_;
  float friction_ratio_;
  qint64 last_time_updated_{0};
  T value_{};
  T velocity_{};
};

template<class T>
SpringEmulator<T>::SpringEmulator(float stiffness_ratio, float friction_ratio) :
    stiffness_ratio_{stiffness_ratio},
    friction_ratio_{friction_ratio},
    last_time_updated_{QDateTime::currentMSecsSinceEpoch()} {}

template<class T>
void SpringEmulator<T>::MakeStep(const T& new_value) {
  // Lets divide dt by 10 so the constants wont be too small
  auto time = QDateTime::currentMSecsSinceEpoch();
  auto delta_time = static_cast<float>(time - last_time_updated_) / 10.f;
  last_time_updated_ = time;

  if (delta_time > 5.f) {
    qWarning() << "[SPRING EMULATOR] delta_time is too big to make step";
    value_ = new_value;
    return;
  }

  // By the Hooke's Law: F = -k * x where x = |A - B| - l_0.
  // Owr "spring's" length is 0, so l_0 = 0
  T f = -stiffness_ratio_ * (value_ - new_value);
  // v = F*dt/m (let m = 1)
  velocity_ += f * delta_time;
  velocity_ -= velocity_ * friction_ratio_ * delta_time;
  value_ += velocity_ * delta_time;
}

template<class T>
void SpringEmulator<T>::SetValue(const T& last_value) {
  value_ = last_value;
  last_time_updated_ = QDateTime::currentMSecsSinceEpoch();
}

template<class T>
T SpringEmulator<T>::GetValue() const {
  return value_;
}

#endif  // SPRINGEMULATOR_SPRING_EMULATOR_H_
