#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>
#include <memory>

#include <QVector2D>
#include <QEvent>

#include "GameObject/MovableObject/Entity/entity.h"
#include "Weapon/Crossbow/crossbow.h"
#include "Weapon/MachineGun/machine_gun.h"
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

  std::shared_ptr<GameObject> Clone() const override;

 private:
  bool is_local_player_{false};
  std::shared_ptr<Weapon> weapon_;
  WeaponType weapon_type_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
