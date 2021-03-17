#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

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

  EventType GetType() const;
  QVariant GetArg(int index) const;
  template<typename T>
  T GetArg(int index) const {
    return args_.at(index).value<T>();
  }
  std::vector<QVariant> GetArgs() const;
  // Gets sub-vector of |args_| elements in range [first_index, max_index]
  std::vector<QVariant> GetArgsSubVector(int first_index) const;
  // Gets sub-vector of |args_| elements in range [first_index, last_index)
  std::vector<QVariant> GetArgsSubVector(int first_index, int last_index) const;

  QByteArray ToByteArray() const;

  friend QDebug operator<<(QDebug debug, const Event& event);

 private:
  EventType type_;
  std::vector<QVariant> args_;
};

#endif  // EVENT_EVENT_H_
