#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_

#include <memory>
#include <vector>

#include "GameObject/MovableObject/Entity/entity.h"
#include "GameObject/MovableObject/Entity/Player/player.h"
#include "GameObject/MovableObject/Entity/Creep/creep_enums.h"

class Creep : public Entity {
 public:
  explicit Creep(GameObjectId player_id);
  ~Creep() override = default;

  void DrawRelatively(Painter* painter) const override;

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
  QPointF GetSpawnPoint() const;

  void SetAttackDistance(float attack_distance);
  float GetAttackDistance() const;

  void SetDamage(float damage);
  float GetDamage() const;

  bool IsPossibleToAttack(int64_t timestamp) const;

  void SetLastAttackedTime(int64_t last_attacked_time);
  int64_t GetLastAttackedTime() const;

  void SetReloadingTime(int64_t reloading_time);
  int64_t GetReloadingTime() const;

  void SetCreepType(CreepType creep_type);
  CreepType GetCreepType() const;

  QVector2D GetForce() const;

  void SetAggressivePlayer(const std::shared_ptr<Player>& player);

  void TickIntelligence(
      const std::vector<std::shared_ptr<GameObject>>& game_objects);

  GameObjectId GetPlayerToDamage(
      const std::vector<std::shared_ptr<Player>>& players) const;

 private:
  QPointF GetPatrollingPoint(
      const std::vector<std::shared_ptr<GameObject>>& near_game_objects);

  float exp_increment_{};
  float spawn_x_;
  float spawn_y_;
  float attack_distance_;
  float damage_;
  QVector2D force_;
  int64_t last_attacked_time_{0};
  int64_t reloading_time_;
  CreepType creep_type_{CreepType::kNone};
  GameObjectId last_aggressive_player_;
  int64_t last_taken_damage_time_{0};
  QPointF patrolling_point_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
