#ifndef GAMEOBJECT_BOX_H_
#define GAMEOBJECT_BOX_H_

#include <vector>
#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

class Box : public GameObject {
 public:
  Box(GameObjectId game_object_id,
      float x, float y, float rotation, float width, float height);
  Box(GameObjectId game_object_id, const std::vector<QVariant>& params);


  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;

  void SetParams(const std::vector<QVariant>& params) override;
  std::vector<QVariant> GetParams() const override;

  GameObjectType GetType() const override;
};

#endif  // GAMEOBJECT_BOX_H_
