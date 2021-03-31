#ifndef GAMEOBJECT_TREE_H_
#define GAMEOBJECT_TREE_H_

#include <vector>
#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RigidBody/rigid_body_circle.h"

class Tree : public GameObject {
 public:
  Tree(GameObjectId game_object_id, float x, float y, float radius);
  Tree(GameObjectId game_object_id, const std::vector<QVariant>& params);

  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;

  void SetRadius(float radius);
  float GetRadius() const;

  void SetParams(const std::vector<QVariant>& params) override;
  std::vector<QVariant> GetParams() const override;

  GameObjectType GetType() const override;
};

#endif  // GAMEOBJECT_TREE_H_
