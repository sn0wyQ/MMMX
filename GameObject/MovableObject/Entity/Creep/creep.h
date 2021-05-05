#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_

#include <memory>
#include <vector>

#include "GameObject/MovableObject/Entity/entity.h"

class Creep : public Entity {
 public:
  explicit Creep(GameObjectId player_id);

  void DrawRelatively(Painter* painter) override;

  GameObjectType GetType() const override;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetExpIncrementForKill(float exp_increment);
  float GetExpIncrementForKill() const override;
  float& GetMutableExpIncrementForKill();

  std::shared_ptr<GameObject> Clone() const override;

  void SetSpawnX(float spawn_x);
  float GetSpawnX() const;

  void SetSpawnY(float spawn_y);
  float GetSpawnY() const;

  void SetIsGoingToSpawn(bool is_going_to_spawn);
  bool IsGoingToSpawn() const;

  void SetAttackDistance(float attack_distance);
  float GetAttackDistance() const;

  void SetDamage(float damage);
  float GetDamage() const;

  bool IsPossibleToAttack(int64_t timestamp) const;

  void SetLastAttackedTime(int64_t last_attacked_time);
  int64_t GetLastAttackedTime() const;

  void SetReloadingTime(int64_t reloading_time);
  int64_t GetReloadingTime() const;

 private:
  float exp_increment_{};
  float spawn_x_;
  float spawn_y_;
  bool is_going_to_spawn_{false};
  float attack_distance_;
  float damage_;
  int64_t last_attacked_time_;
  int64_t reloading_time_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
