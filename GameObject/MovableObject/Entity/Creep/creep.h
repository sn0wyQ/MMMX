#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_

#include <memory>
#include <vector>

#include "GameObject/MovableObject/Entity/entity.h"

class Creep : public Entity {
 public:
  explicit Creep(GameObjectId player_id);

  void DrawRelatively(Painter* painter) override;
  void DrawLevel(Painter* painter) override;

  GameObjectType GetType() const override;

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetExpIncrementForKill(float exp_increment);
  float GetExpIncrementForKill() const override;
  float& GetMutableExpIncrementForKill();

  std::shared_ptr<GameObject> Clone() const override;

 private:
  float exp_increment_{};
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_CREEP_CREEP_H_
