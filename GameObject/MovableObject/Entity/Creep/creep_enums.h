#ifndef CREEP_ENUMS_H_
#define CREEP_ENUMS_H_

namespace CreepEnumsWrapper {

Q_NAMESPACE

enum class CreepType {
  kBoxSciFiCube,
  kBoxSciFiLong,
  kTemporaryCircle
};

Q_ENUM_NS(CreepType)

}  // namespace CreepEnumsWrapper

using CreepType = CreepEnumsWrapper::CreepType;
Q_DECLARE_METATYPE(CreepType)

#endif  // CREEP_ENUMS_H_
