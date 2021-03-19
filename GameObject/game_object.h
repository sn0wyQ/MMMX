#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <QPoint>

#include "constants.h"
#include "GameObject/Rigidbody/rigidbody.h"
#include "GameObject/Rigidbody/rigidbody_circle.h"
#include "GameObject/Rigidbody/rigidbody_rectangle.h"
#include "Painter/painter.h"

class GameObject {
 public:
  // GameObject(GameObjectId id, const RigidbodyCircle& rigidbody_circle);
  // GameObject(GameObjectId id,
  //                     const RigidbodyRectangle& rigidbody_rectangle);
  GameObject(GameObjectId id, std::shared_ptr<Rigidbody> rigidbody);

  virtual void OnTick(int time_from_previous_tick) = 0;
  virtual void Draw(Painter* painter) = 0;

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
  std::shared_ptr<Rigidbody> rigidbody_;
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
