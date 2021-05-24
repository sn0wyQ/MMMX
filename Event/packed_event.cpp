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
  auto size = ByteArrayToInt32(byte_array.left(4));
  cache_.reserve(size);
  int now = 4;
  for (int i = 0; i < size; i++) {
    auto bytes_size = ByteArrayToInt32(byte_array.left(now + 4).right(4));
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
  result.push_back(Int32ToByteArray(cache_.size()));
  for (const auto& event : cache_) {
    auto event_bytes = event.ToByteArray();
    result.push_back(Int32ToByteArray(event_bytes.size()));
    result.push_back(event_bytes);
  }
  return result;
}

std::vector<Event> PackedEvent::GetEvents() const {
  return cache_;
}

void PackedEvent::Clear() {
  cache_.clear();
}

QByteArray PackedEvent::Int32ToByteArray(uint32_t n) {
  QByteArray result;
  uint32_t mask = (1LL << 8) - 1;
  result.push_back(static_cast<char>((n >> 24) & mask));
  result.push_back(static_cast<char>((n >> 16) & mask));
  result.push_back(static_cast<char>((n >> 8) & mask));
  result.push_back(static_cast<char>(n & mask));
  return result;
}

uint32_t PackedEvent::ByteArrayToInt32(const QByteArray& byte_array) {
  auto res = static_cast<uint32_t>(static_cast<uchar>(byte_array[3]));
  res |= static_cast<uint32_t>(static_cast<uchar>(byte_array[2])) << 8;
  res |= static_cast<uint32_t>(static_cast<uchar>(byte_array[1])) << 16;
  res |= static_cast<uint32_t>(static_cast<uchar>(byte_array[0])) << 24;
  return res;
}
