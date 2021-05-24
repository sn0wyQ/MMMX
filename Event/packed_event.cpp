#include <iostream>
#include "packed_event.h"

void Out(const QByteArray& byte_array) {
  std::cout << "[";
  for (char i : byte_array) {
    std::cout << (uint32_t)(uchar)i << ' ';
  }
  std::cout << "]\n";
}

PackedEvent::PackedEvent(const QByteArray& byte_array) {
  uint32_t size;
  {
    QDataStream data_stream(byte_array.left(4));
    data_stream >> size;
  }
  cache_.reserve(size);
  int now = 4;
  for (uint32_t i = 0; i < size; i++) {
    uint32_t bytes_size;
    {
      QDataStream data_stream(byte_array.left(now + 4).right(4));
      data_stream >> bytes_size;
    }
    cache_.emplace_back(
        byte_array.left(now + 4 + bytes_size).right(bytes_size));
    now += bytes_size + 4;
  }
}

void PackedEvent::AddEvent(const Event& event) {
  cache_.push_back(event);
}

QByteArray PackedEvent::ToByteArray() const {
  QByteArray result;
  QDataStream data_stream(&result, QIODevice::ReadWrite);
  data_stream << static_cast<uint32_t>(cache_.size());
  for (const auto& event : cache_) {
    auto event_bytes = event.ToByteArray();
    data_stream << event_bytes;
  }
  return result;
}

std::vector<Event> PackedEvent::GetEvents() const {
  return cache_;
}

void PackedEvent::Clear() {
  cache_.clear();
}
