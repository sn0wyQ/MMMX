#ifndef GAMEOBJECT_BOX_H_
#define GAMEOBJECT_BOX_H_

#include <memory>

#include "constants.h"
#include "GameObject/game_object.h"

class Box : public GameObject {
 public:
  Box(int width, int height);

  void Draw(Painter* painter) override;

 private:
  int width_;
  int height_;
};

#endif  // GAMEOBJECT_BOX_H_
