#ifndef GAMEOBJECT_BOX_H_
#define GAMEOBJECT_BOX_H_

#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"

class Box : public GameObject {
 public:
  Box(GameObjectId game_object_id,
      float x, float y, float width, float height);

  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;

  float GetWidth() const;
  float GetHeight() const;

 private:
  float width_;
  float height_;
};

#endif  // GAMEOBJECT_BOX_H_
