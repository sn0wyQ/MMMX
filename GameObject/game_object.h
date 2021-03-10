#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <QObject>
#include <QPoint>

#include "Animation/animation.h"
#include "constants.h"

class GameObject : public QObject {
 public:
  GameObject() = default;
  explicit GameObject(GameObjectId id);

  virtual void OnTick() = 0;

  GameObjectId GetId();
  void SetId(GameObjectId id);

 private:
  GameObjectId id_ = Constants::kNullGameObjectId;
  QPoint position_;
  Animation animation_;
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
