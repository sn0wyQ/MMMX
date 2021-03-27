#ifndef GAMEOBJECT_RECTANGULARSTATICOBJECT_BOX_H_
#define GAMEOBJECT_RECTANGULARSTATICOBJECT_BOX_H_

#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"
#include "GameObject/RectangularStaticObject/rectangular_static_object.h"

class Box : public RectangularStaticObject {
 public:
  Box(GameObjectId game_object_id,
      float x, float y, float rotation, float width, float height);

  void OnTick(int) override;
  void DrawRelatively(Painter* painter) override;
};

#endif  // GAMEOBJECT_RECTANGULARSTATICOBJECT_BOX_H_
