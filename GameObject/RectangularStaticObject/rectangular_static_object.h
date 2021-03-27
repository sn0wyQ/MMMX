#ifndef GAMEOBJECT_RECTANGULARSTATICOBJECT_RECTANGULAR_STATIC_OBJECT_H_
#define GAMEOBJECT_RECTANGULARSTATICOBJECT_RECTANGULAR_STATIC_OBJECT_H_

#include <utility>
#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

class RectangularStaticObject : public GameObject {
 public:
  RectangularStaticObject(GameObjectId game_object_id,
                          float width, float height,
                          std::shared_ptr<RigidBodyRectangle> rigid_body);

  std::shared_ptr<RigidBodyRectangle> GetRigidBody() const;

  float GetWidth() const;
  float GetHeight() const;

 private:
  float width_;
  float height_;

  std::shared_ptr<RigidBodyRectangle> rigid_body_;
};

#endif  // GAMEOBJECT_RECTANGULARSTATICOBJECT_RECTANGULAR_STATIC_OBJECT_H_
