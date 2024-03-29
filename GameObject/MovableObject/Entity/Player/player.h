#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <algorithm>
#include <vector>
#include <memory>

#include <QVector2D>
#include <QEvent>

#include "GameObject/MovableObject/Entity/entity.h"
#include "GameObject/MovableObject/Entity/Player/player_enums.h"
#include "Weapon/AssaultRifle/assault_rifle.h"
#include "Weapon/Crossbow/crossbow.h"
#include "Weapon/MachineGun/machine_gun.h"
#include "Weapon/Shotgun/shotgun.h"
#include "Constants/constants.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id);
  Player(const Player& other);
  virtual ~Player() = default;

  void DrawRelatively(Painter* painter) const override;
  void DrawNickname(Painter* painter,
                    const QString& nickname) const override;

  bool IsLocalPlayer() const;
  void SetIsLocalPlayer(bool is_local_player);

  void SetRadius(float radius);
  float GetRadius() const;

  bool IsPlayer() const override;

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
  void IncreaseLevelingPoint(LevelingSlots leveling_slot);

  void SetNeedToSendLevelingPoints(bool need_to_send_leveling_points);
  bool IsNeedToSendLevelingPoints() const;

  void Revive(QPointF point_to_spawn) override;

 private:
  bool is_local_player_{false};
  std::shared_ptr<Weapon> weapon_{};
  WeaponType weapon_type_;
  float current_exp_{};
  int free_leveling_points_{};
  std::vector<int> leveling_points_;
  bool need_to_send_leveling_points_{false};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
