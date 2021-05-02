#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <algorithm>
#include <vector>
#include <memory>

#include <QVector2D>
#include <QEvent>

#include "GameObject/MovableObject/Entity/entity.h"
#include "Weapon/AssaultRifle/assault_rifle.h"
#include "Weapon/Crossbow/crossbow.h"
#include "Weapon/MachineGun/machine_gun.h"
#include "Weapon/Shotgun/shotgun.h"
#include "constants.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id);
  Player(const Player& other);

  void DrawRelatively(Painter* painter) override;

  bool IsLocalPlayer() const;
  void SetIsLocalPlayer(bool is_local_player);

  void SetRadius(float radius);
  float GetRadius() const;

  GameObjectType GetType() const override;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  const std::shared_ptr<Weapon>& GetWeapon() const;

  void SetCurrentExp(float current_exp);
  float GetCurrentExp() const;

  void IncreaseExperience(float experience_to_add);

  float GetExpIncrementForKill() const override;

  std::shared_ptr<GameObject> Clone() const override;

  void SetFreeLevelingPoints(int free_leveling_points);
  int GetFreeLevelingPoints() const;

  const std::vector<int>& GetLevelingPoints() const;
  void IncreaseLevelingPoint(int index);

  void SetNeedToSendLevelingPoints(bool need_to_send_leveling_points);
  bool IsNeedToSendLevelingPoints() const;

 private:
  bool is_local_player_{false};
  std::shared_ptr<Weapon> weapon_;
  WeaponType weapon_type_;
  float current_exp_{};
  int free_leveling_points_{};
  std::vector<int> leveling_points_;
  bool need_to_send_leveling_points_{false};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
