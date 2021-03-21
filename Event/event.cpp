#include "event.h"

Event::Event(const Event& event) {
  type_ = event.type_;
  args_ = event.args_;
}

Event::Event(const QByteArray& message) {
  QDataStream data_stream(message);
  int type_in_int;
  data_stream >> type_in_int;
  type_ = static_cast<EventType>(type_in_int);
  while (!data_stream.atEnd()) {
    QVariant next_variable;
    data_stream >> next_variable;
    args_.push_back(next_variable);
  }
}

EventType Event::GetType() const {
  return type_;
}

QVariant Event::GetArg(int index) const {
  return args_.at(index);
}

void Event::PushBackArgsFromEvent(const Event& other) {
  auto args = other.GetArgs();
  for (auto& arg : args) {
    args_.push_back(arg);
  }
}

std::vector<QVariant> Event::GetArgs() const {
  return args_;
}

std::vector<QVariant> Event::GetArgsSubVector(int first_index) const {
  return std::vector<QVariant>(args_.begin() + first_index,
                               args_.end());
}

std::vector<QVariant>
    Event::GetArgsSubVector(int first_index, int last_index) const {
  return std::vector<QVariant>(args_.begin() + first_index,
                               args_.begin() + last_index);
}

QByteArray Event::ToByteArray() const {
  QByteArray result;
  QDataStream data_stream(&result, QIODevice::WriteOnly);
  data_stream << static_cast<int>(type_);
  for (const auto& arg : args_) {
    data_stream << arg;
  }
  return result;
}

QDebug operator<<(QDebug debug, const Event& event) {
  const bool oldSetting = debug.autoInsertSpaces();
  debug.nospace() << "Event(Type: "
      << QString(QMetaEnum::fromType<EventType>()
          .valueToKey(static_cast<uint32_t>(event.type_)));
  for (const auto& arg : event.args_) {
    debug.nospace() << ", ";
    switch (static_cast<QMetaType::Type>(arg.type())) {
      case QMetaType::Int:
        debug.nospace() << arg.toInt();
        break;

      case QMetaType::UInt:
        debug.nospace() << "0b" << Qt::bin << arg.toUInt();
        break;

      case QMetaType::Float: {
        float value = arg.toFloat();
        debug.nospace() << value << (std::ceil(value) == value ? ".0f" : "f");
        break;
      }

      default:
        debug.nospace() << arg;
        break;
    }
  }
  debug.nospace() << ')';
  debug.setAutoInsertSpaces(oldSetting);
  return debug.maybeSpace();
}
