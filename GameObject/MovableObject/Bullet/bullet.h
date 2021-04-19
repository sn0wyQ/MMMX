#ifndef GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
#define GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_

#include "GameObject/MovableObject/movable_object.h"

class Bullet : public MovableObject {
 public:
  explicit Bullet(GameObjectId bullet_id);

  virtual GameObjectType GetType() const;

  void DrawRelatively(Painter* painter) override;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetParentId(GameObjectId parent_id);
  GameObjectId GetParentId() const;
  void SetStartPosition(QPointF start_position);
  QPointF GetStartPosition() const;

  std::shared_ptr<GameObject> Clone() const override;

 private:
  GameObjectId parent_id_{Constants::kNullGameObjectId};
  QPointF start_position_{0.f, 0.f};

};
#endif  // GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
