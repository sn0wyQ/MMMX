#ifndef GAME_OBJECT_MOVABLE_OBJECT_MOVABLE_OBJECT_H_
#define GAME_OBJECT_MOVABLE_OBJECT_MOVABLE_OBJECT_H_

#include "GameObject/game_object.h"

class MovableObject : public GameObject {
  Q_OBJECT

 public:
  MovableObject() = default;

  void OnTick() override = 0;

 private:

};

#endif  // GAME_OBJECT_MOVABLE_OBJECT_MOVABLE_OBJECT_H_
