#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

#include <utility>
#include <vector>

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QObject>

#include "Event/event_type.h"

class Event : public QObject {
  Q_OBJECT
  Q_ENUM(EventType)
  Q_PROPERTY(EventType event_type MEMBER type_)
  Q_PROPERTY(std::vector<int> event_args MEMBER args_)

 public:
  Event(const Event& event);
  explicit Event(const QByteArray& message);
  template<typename... Args>
  explicit Event(EventType event_type, Args... args)
    : type_(event_type), args_({args...}) {}

  EventType GetType() const;
  int GetArg(int index) const;
  std::vector<int> GetArgs() const;
  // Gets sub-vector of |args_| elements in range [first_index, max_index]
  std::vector<int> GetArgsSubVector(int first_index) const;
  // Gets sub-vector of |args_| elements in range [first_index, last_index)
  std::vector<int> GetArgsSubVector(int first_index, int last_index) const;

  QByteArray ToByteArray() const;

  friend QDebug operator<<(QDebug debug, const Event& event);

 private:
  EventType type_;
  std::vector<int> args_;
};

#endif  // EVENT_EVENT_H_
