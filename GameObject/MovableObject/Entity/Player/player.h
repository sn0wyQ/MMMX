#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_

#include <vector>
#include <memory>

#include <QVector2D>

#include "GameObject/MovableObject/Entity/entity.h"
#include "constants.h"

class Player : public Entity {
 public:
  explicit Player(GameObjectId player_id, float radius);
  Player(GameObjectId player_id, float x, float y, float radius);
  Player(GameObjectId player_id, const std::vector<QVariant>& params);

  void DrawRelatively(Painter* painter) override;

  bool IsLocalPlayer() const;
  void SetIsLocalPlayer(bool is_local_player);

  void SetRadius(float radius);
  float GetRadius() const;

  void SetParams(const std::vector<QVariant>& params) override;
  std::vector<QVariant> GetParams() const override;

  GameObjectType GetType() const override;
  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

 private:
  float fov_radius_{Constants::kDefaultPlayersFov};
  bool is_local_player_{false};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_PLAYER_H_
