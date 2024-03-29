#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

#include <cinttypes>
#include <cmath>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QMetaEnum>
#include <QVariant>

#include "Event/event_type.h"

class Event {
 public:
  Event(const Event& event);
  template<typename... Args>
  explicit Event(EventType event_type, const Args&... args)
    : type_(event_type) {
    args_ = { args... };
  }
  explicit Event(const QByteArray& message);
  explicit Event(const QList<QVariant>& list);

  EventType GetType() const;
  QVariant GetArg(int index) const;
  template<typename T>
  T GetArg(int index) const {
    return args_.at(index).value<T>();
  }
  template<typename T>
  void SetArg(int index, const T& value) {
    args_.at(index).setValue<T>(value);
  }
  static Event CreateSendToClientEvent(const Event& event);
  // Pushes back arguments (not type) into this event |args_|
  void AddArgsFromEvent(const Event& other);
  template<typename T>
  T GetLastArg() const {
    return args_.back().value<T>();
  }
  std::vector<QVariant> GetArgs() const;
  // Gets sub-vector of |args_| elements in range [first_index, max_index]
  std::vector<QVariant> GetArgsSubVector(int first_index) const;
  // Gets sub-vector of |args_| elements in range [first_index, last_index)
  std::vector<QVariant> GetArgsSubVector(int first_index, int last_index) const;

  void PushBackArg(const QVariant& variant) {
    args_.push_back(variant);
  }

  void PushBackArgs(const std::vector<QVariant>& vector) {
    for (const auto& variant : vector) {
      args_.push_back(variant);
    }
  }

  template<typename T>
  void PushBackArg(const T& value) {
    args_.emplace_back(value);
  }

  template<typename T>
  T PopBackArg() {
    T value = args_.back().value<T>();
    args_.pop_back();
    return value;
  }

  QByteArray ToByteArray() const;
  QList<QVariant> ToQList() const;

  friend QDebug operator<<(QDebug debug, const Event& event);

 private:
  EventType type_;
  std::vector<QVariant> args_;
};

#endif  // EVENT_EVENT_H_
