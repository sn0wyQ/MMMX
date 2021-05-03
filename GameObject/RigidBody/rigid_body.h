#ifndef GAMEOBJECT_RIGIDBODY_RIGID_BODY_H_
#define GAMEOBJECT_RIGIDBODY_RIGID_BODY_H_

#include <memory>

#include "Painter/painter.h"

namespace RigidBodyTypeWrapper {

Q_NAMESPACE

enum class RigidBodyType {
  kCircle,
  kRectangle
};

Q_ENUM_NS(RigidBodyType)

}  // namespace RigidBodyTypeWrapper

using RigidBodyType = RigidBodyTypeWrapper::RigidBodyType;

Q_DECLARE_METATYPE(RigidBodyType)

class RigidBody {
 public:
  RigidBody() = default;
  RigidBody(float width, float height);

  float GetWidth() const;
  float GetHeight() const;

  void SetWidth(float width);
  void SetHeight(float height);

  virtual RigidBodyType GetType() const = 0;

  virtual std::shared_ptr<RigidBody> External(float friction_force) const = 0;

  virtual void Draw(Painter* painter) = 0;

 private:
  float width_{};
  float height_{};
};

#endif  // GAMEOBJECT_RIGIDBODY_RIGID_BODY_H_
