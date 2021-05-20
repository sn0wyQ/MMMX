#ifndef GUI_ANIMATIONS_SPRING_EMULATOR_H_
#define GUI_ANIMATIONS_SPRING_EMULATOR_H_

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QVector2D>

template <class T, bool DoSetBounds>
class SpringEmulator {
 public:
  SpringEmulator(float stiffness_ratio, float friction_ratio_);

  void MakeStepTo(const T& new_value);

  void SetCurrentValue(const T& last_value);
  T GetCurrentValue() const;

  void SetBounds(T min, T max);

 private:
  float stiffness_ratio_;
  float friction_ratio_;
  qint64 last_time_updated_{0};
  T value_{};
  T velocity_{};
  T min_value_{};
  T max_value_{};
};

template <class T, bool DoSetBounds>
SpringEmulator<T, DoSetBounds>::SpringEmulator(float stiffness_ratio,
                                               float friction_ratio) :
    stiffness_ratio_{stiffness_ratio},
    friction_ratio_{friction_ratio},
    last_time_updated_{QDateTime::currentMSecsSinceEpoch()} {}

template <class T, bool DoSetBounds>
void SpringEmulator<T, DoSetBounds>::MakeStepTo(const T& new_value) {
  // Lets divide dt by 10 so the constants wont be too small
  auto time = QDateTime::currentMSecsSinceEpoch();
  auto delta_time = static_cast<float>(time - last_time_updated_) / 10.f;
  last_time_updated_ = time;

  float sample_rate = 0.4f;
  for (float now = 0.f; now < delta_time; now += sample_rate) {
    // By the Hooke's Law: F = -k * x where x = |A - B| - l_0.
    // Owr "spring's" length is 0, so l_0 = 0
    T f = -stiffness_ratio_ * (value_ - new_value);
    // v = F*dt/m (let m = 1)
    velocity_ += f * sample_rate;
    velocity_ -= velocity_ * friction_ratio_ * sample_rate;
    value_ += velocity_ * sample_rate;
  }
}

template <class T, bool DoSetBounds>
void SpringEmulator<T, DoSetBounds>::SetCurrentValue(const T& last_value) {
  value_ = last_value;
  last_time_updated_ = QDateTime::currentMSecsSinceEpoch();
}

template <class T, bool DoSetBounds>
T SpringEmulator<T, DoSetBounds>::GetCurrentValue() const {
  if constexpr (DoSetBounds) {
    return std::max(std::min(value_, max_value_), min_value_);
  }
  return value_;
}

template <class T, bool DoSetBounds>
void SpringEmulator<T, DoSetBounds>::SetBounds(T min, T max) {
  static_assert(DoSetBounds,
      "You can't use SetPath with second template parameter 'false'");
  min_value_ = min;
  max_value_ = max;
}

#endif  // GUI_ANIMATIONS_SPRING_EMULATOR_H_
