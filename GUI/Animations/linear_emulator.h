#ifndef GUI_ANIMATIONS_LINEAR_EMULATOR_H_
#define GUI_ANIMATIONS_LINEAR_EMULATOR_H_

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QVector2D>

template<class T>
class LinearEmulator {
 public:
  explicit LinearEmulator(float speed);

  void SetStopOnMin(bool do_stop);
  void SetStopOnMax(bool do_stop);

  void SetCurrentValue(const T& value);
  T GetCurrentValue();

  void SetPath(T start, T finish);

 private:
  float speed_;
  bool stop_on_min_{false};
  bool stop_on_max_{false};
  qint64 last_time_updated_{0};
  T value_{};
  T min_value_{};
  T max_value_{};
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
T LinearEmulator<T>::GetCurrentValue() {
  // Lets divide dt by 10 so the constants wont be too small
  auto time = QDateTime::currentMSecsSinceEpoch();
  auto delta_time = static_cast<float>(time - last_time_updated_) / 10.f;
  last_time_updated_ = time;

  float sample_rate = 1.f;
  for (float now = 0; now < delta_time; now += sample_rate) {
    if (value_ > max_value_) {
      if (stop_on_max_) {
        break;
      }
      speed_ = -abs(speed_);
    }
    if (value_ < min_value_) {
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
  min_value_ = std::min(start, finish);
  max_value_ = std::max(start, finish);
  if (start < finish) {
    speed_ = abs(speed_);
  } else {
    speed_ = -abs(speed_);
  }
}

#endif  // GUI_ANIMATIONS_LINEAR_EMULATOR_H_
