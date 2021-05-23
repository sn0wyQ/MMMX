#ifndef CLIENT_GUI_ANIMATIONS_LINEAR_EMULATOR_H_
#define CLIENT_GUI_ANIMATIONS_LINEAR_EMULATOR_H_

#include "../../../../../Qt/Tools/mingw810_64/lib/gcc/x86_64-w64-mingw32/8.1.0/include/c++/algorithm"

#include "../../../../../Qt/5.15.2/mingw81_64/include/QtCore/QDateTime"
#include "../../../../../Qt/5.15.2/mingw81_64/include/QtCore/QDebug"
#include "../../../../../Qt/5.15.2/mingw81_64/include/QtGui/QVector2D"

#include "../../../Math/math.h"

template<class T>
class LinearEmulator {
 public:
  explicit LinearEmulator(float speed);

  void SetStopOnMin(bool do_stop);
  void SetStopOnMax(bool do_stop);

  void SetCurrentValue(const T& value);
  T GetCurrentValue() const;

  void SetPath(T start, T finish);

 private:
  mutable float speed_;
  bool stop_on_min_{false};
  bool stop_on_max_{false};
  mutable qint64 last_time_updated_{0};
  mutable T value_{};
  T min_value_{};
  T max_value_{};
  T last_start_{};
  T last_finish_{};
};

template<class T>
LinearEmulator<T>::LinearEmulator(float speed) :
    speed_(speed),
    last_time_updated_{QDateTime::currentMSecsSinceEpoch()} {}

template<class T>
void LinearEmulator<T>::SetStopOnMin(bool do_stop) {
  stop_on_min_ = do_stop;
}

template<class T>
void LinearEmulator<T>::SetStopOnMax(bool do_stop) {
  stop_on_max_ = do_stop;
}

template<class T>
void LinearEmulator<T>::SetCurrentValue(const T& value) {
  value_ = value;
}

template<class T>
T LinearEmulator<T>::GetCurrentValue() const {
  // Lets divide it by 10 so the constants won't be too small
  auto time = QDateTime::currentMSecsSinceEpoch();
  auto delta_time = static_cast<float>(time - last_time_updated_) / 10.f;
  last_time_updated_ = time;

  float sample_rate = 0.1f;
  for (float now = 0; now < delta_time; now += sample_rate) {
    if (value_ > max_value_ && speed_ > 0.f) {
      value_ = max_value_;
      if (stop_on_max_) {
        break;
      }
      speed_ = -abs(speed_);
    }
    if (value_ < min_value_ && speed_ < 0.f) {
      value_ = min_value_;
      if (stop_on_min_) {
        break;
      }
      speed_ = abs(speed_);
    }
    value_ += sample_rate * speed_;
  }

  return std::max(min_value_, std::min(value_, max_value_));
}

template<class T>
void LinearEmulator<T>::SetPath(T start, T finish) {
  if (std::fabs(last_start_ - start) > Math::kEps ||
      std::fabs(last_finish_ - finish) > Math::kEps) {
    last_time_updated_ = QDateTime::currentMSecsSinceEpoch();
  }
  last_start_ = start;
  last_finish_ = finish;
  min_value_ = std::min(start, finish);
  max_value_ = std::max(start, finish);
  if (start < finish) {
    speed_ = std::fabs(speed_);
  } else {
    speed_ = -std::fabs(speed_);
  }
}

#endif  // CLIENT_GUI_ANIMATIONS_LINEAR_EMULATOR_H_
