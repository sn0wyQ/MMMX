#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <QObject>
#include <QPoint>

#include "constants.h"

class GameObject {
 public:
  explicit GameObject(GameObjectId id);

  virtual void OnTick() = 0;

  GameObjectId GetId();
  void SetId(GameObjectId id);

  QPoint GetPosition() const;
  void SetPosition(QPoint point);
  int GetX() const;
  void SetX(int x);
  int GetY() const;
  void SetY(int y);

 private:
  GameObjectId id_ = Constants::kNullGameObjectId;
  QPoint position_{0, 0};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
