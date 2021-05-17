#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_ENUMS_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_ENUMS_H_

namespace CreepEnumsWrapper {

Q_NAMESPACE

enum class CreepType {
  kNone = -1,
  kGenerator,
  kGiant1,
  kGiant2,
  kBox,
};

Q_ENUM_NS(CreepType)

}  // namespace CreepEnumsWrapper

using CreepType = CreepEnumsWrapper::CreepType;
Q_DECLARE_METATYPE(CreepType)

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_ENUMS_H_
