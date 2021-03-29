#ifndef GAMEOBJECT_ROUNDSTATICOBJECT_ROUND_STATIC_OBJECT_H_
#define GAMEOBJECT_ROUNDSTATICOBJECT_ROUND_STATIC_OBJECT_H_

#include <utility>
#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body_circle.h"

class RoundStaticObject : public GameObject {
 public:
  RoundStaticObject(
      GameObjectId game_object_id, float radius,
      std::shared_ptr<RigidBodyCircle> rigid_body);

  std::shared_ptr<RigidBodyCircle> GetRigidBody() const;

  float GetRadius() const;
  virtual float GetFrictionForce() const;

 private:
  float radius_;

  std::shared_ptr<RigidBodyCircle> rigid_body_;
};

#endif  // GAMEOBJECT_ROUNDSTATICOBJECT_ROUND_STATIC_OBJECT_H_
