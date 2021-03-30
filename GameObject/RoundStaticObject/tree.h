#ifndef GAMEOBJECT_ROUNDSTATICOBJECT_TREE_H_
#define GAMEOBJECT_ROUNDSTATICOBJECT_TREE_H_

#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RoundStaticObject/round_static_object.h"

class Tree : public RoundStaticObject {
 public:
  Tree(GameObjectId game_object_id, float x, float y, float radius);
  Tree(GameObjectId game_object_id, const std::vector<QVariant>& params);

  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;

  void SetParams(const std::vector<QVariant>& params) override;
  std::vector<QVariant> GetParams() const override;

  GameObjectType GetGameObjectType() const override;
};

#endif  // GAMEOBJECT_ROUNDSTATICOBJECT_TREE_H_
