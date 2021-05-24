#ifndef PACKED_EVENT_H_
#define PACKED_EVENT_H_

#include "Event/event.h"

class PackedEvent {
 public:
  PackedEvent() = default;
  explicit PackedEvent(const QByteArray& byte_array);

  void AddEvent(const Event& event);
  QByteArray ToByteArray() const;
  std::vector<Event> GetEvents() const;
  void Clear();

 private:
  static QByteArray Uint32ToByteArray(uint32_t n);
  static uint32_t ByteArrayToUint32(const QByteArray& byte_array);

  std::vector<Event> cache_;
};

#endif  // PACKED_EVENT_H_
