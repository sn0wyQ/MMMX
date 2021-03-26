#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>

#include <QVector2D>

#include "GameObject/MovableObject/Entity/entity.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id);
  Player(GameObjectId player_id,
         float x,
         float y,
         float rotation);

  void OnTick(int time_from_previous_tick) override;
  void DrawRelatively(Painter* painter) override;

  bool IsLocalPlayer() const;
  void SetIsLocalPlayer(bool is_local_player);

 private:
  bool is_local_player_ = false;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
