#ifndef GAMEOBJECT_TREE_H_
#define GAMEOBJECT_TREE_H_

#include "constants.h"
#include "GameObject/game_object.h"

class Tree : public GameObject {
 public:
  Tree(GameObjectId game_object_id,
      float x, float y, float radius);

  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;

  float GetRadius() const;

 private:
  float radius_;
};

#endif  // GAMEOBJECT_TREE_H_