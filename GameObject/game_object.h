#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <utility>

#include <QPoint>
#include <QDebug>

#include "constants.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "Painter/painter.h"

// enum for sending events
enum class GameObjectTypeForEvents {
  kBox,
  kTree
};

class GameObject {
 public:
  explicit GameObject(GameObjectId id);
  GameObject(GameObjectId id, QPointF position, float rotation);

  virtual void OnTick(int time_from_previous_tick) = 0;
  void Draw(Painter* painter);
  virtual void DrawRelatively(Painter* painter) = 0;

  GameObjectId GetId() const;
  void SetId(GameObjectId id);

  QPointF GetPosition() const;
  void SetPosition(QPointF point);
  float GetX() const;
  void SetX(float x);
  float GetY() const;
  void SetY(float y);
  float GetRotation() const;
  void SetRotation(float rotation);

 private:
  GameObjectId id_ = Constants::kNullGameObjectId;
  QPointF position_{0.f, 0.f};
  float rotation_{0.f};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
