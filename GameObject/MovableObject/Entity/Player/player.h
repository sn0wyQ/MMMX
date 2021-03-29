#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>

#include <QVector2D>

#include "GameObject/MovableObject/Entity/entity.h"
#include "constants.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id);
  Player(GameObjectId player_id,
         float x,
         float y,
         float view_angle);

  void OnTick(int time_from_previous_tick) override;
  void Draw(Painter* painter) override;
  bool IsFilteredByFov() const override;

  bool IsLocalPlayer() const;
  void SetIsLocalPlayer(bool is_local_player);

  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

  void UpdateVelocity(uint32_t mask);

  bool IsInFov() const;
  void SetIsInFov(bool is_in_fov);

 private:
  float fov_radius_{Constants::kDefaultPlayersFov};
  bool is_local_player_ = false;
  bool is_in_fov_{true};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
