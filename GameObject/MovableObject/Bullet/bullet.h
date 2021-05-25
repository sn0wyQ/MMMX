#ifndef GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
#define GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_

#include <vector>
#include <memory>

#include "GameObject/MovableObject/movable_object.h"

class Bullet : public MovableObject {
 public:
  explicit Bullet(GameObjectId bullet_id);
  virtual ~Bullet() = default;

  GameObjectType GetType() const override;

  void OnTick(int delta_time) override;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void CheckRange();

  float GetOpacity() const override;

  void SetParentId(GameObjectId parent_id);
  GameObjectId GetParentId() const;

  void SetStartPosition(QPointF start_position);
  QPointF GetStartPosition() const;

  float GetBulletDamage() const;
  void SetBulletDamage(float bullet_damage);
  float GetBulletSpeed() const;
  void SetBulletSpeed(float bullet_speed);
  float GetBulletRange() const;
  void SetBulletRange(float bullet_range);

  std::shared_ptr<GameObject> Clone() const override;

 private:
  GameObjectId parent_id_{Constants::kNullGameObjectId};
  QPointF start_position_{0.f, 0.f};
  float bullet_damage_{};
  float bullet_speed_{};
  float bullet_range_{};
};
#endif  // GAMEOBJECT_MOVABLEOBJECT_BULLET_BULLET_H_
