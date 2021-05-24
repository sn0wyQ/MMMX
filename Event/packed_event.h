#ifndef EVENT_PACKED_EVENT_H_
#define EVENT_PACKED_EVENT_H_

#include "Event/event.h"

#include <vector>

class PackedEvent {
 public:
  PackedEvent() = default;
  explicit PackedEvent(const QByteArray& byte_array);

  void AddEvent(const Event& event);
  QByteArray ToByteArray() const;
  std::vector<Event> GetEvents() const;
  void Clear();

 private:
  std::vector<Event> cache_;
};

#endif  // EVENT_PACKED_EVENT_H_
