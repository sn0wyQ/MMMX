#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <QPoint>

#include "Painter/painter.h"
#include "constants.h"

class GameObject {
 public:
  explicit GameObject(GameObjectId id);

  virtual void OnTick(int time_from_previous_tick) = 0;
  virtual void Draw(Painter* painter) = 0;
  virtual bool IsFilteredByFOV() const = 0;

  GameObjectId GetId() const;
  void SetId(GameObjectId id);

  QPointF GetPosition() const;
  void SetPosition(QPointF point);
  float GetX() const;
  void SetX(float x);
  float GetY() const;
  void SetY(float y);

 private:
  GameObjectId id_ = Constants::kNullGameObjectId;
  QPointF position_{0.f, 0.f};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
