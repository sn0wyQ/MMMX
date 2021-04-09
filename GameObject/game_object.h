#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <vector>
#include <utility>
#include <memory>

#include <QPoint>
#include <QDebug>

#include "constants.h"
#include "Painter/painter.h"
#include "Math/math.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"

namespace GameObjectTypeWrapper {

Q_NAMESPACE

// enum for sending events
enum class GameObjectType {
  kPlayer,
  kGameObject
};

Q_ENUM_NS(GameObjectType)

}  // namespace GameObjectTypeWrapper

using GameObjectType = GameObjectTypeWrapper::GameObjectType;
Q_DECLARE_METATYPE(GameObjectType)

class GameObject {
 public:
  explicit GameObject(GameObjectId id);

  virtual void OnTick(int delta_time) {}
  void Draw(Painter* painter);
  virtual void DrawRelatively(Painter* painter) {}
  virtual bool IsMovable() const;

  std::shared_ptr<RigidBody> GetRigidBody() const;
  virtual float GetFrictionForce() const;

  GameObjectId GetId() const;

  QPointF GetPosition() const;
  void SetPosition(QPointF point);
  float GetX() const;
  void SetX(float x);
  float GetY() const;
  void SetY(float y);
  float GetRotation() const;
  void SetRotation(float rotation);

  float GetWidth() const;
  float GetHeight() const;
  void SetWidth(float width);
  void SetHeight(float height);

  virtual GameObjectType GetType() const;

  virtual void SetParams(std::vector<QVariant> params);
  virtual std::vector<QVariant> GetParams() const;

  bool IsInFov() const;
  void SetIsInFov(bool is_in_fov);
  virtual bool IsFilteredByFov() const;

 private:
  GameObjectId id_{Constants::kNullGameObjectId};
  bool is_params_set_{false};
  QPointF previous_position_{0.f, 0.f};
  QPointF position_{0.f, 0.f};
  // 0 is direction from left to right
  // Increasing counterclockwise
  // Measured in degrees
  float rotation_{0.f};
  float width_{0.f};
  float height_{0.f};
  std::shared_ptr<RigidBody> rigid_body_;
  bool is_in_fov_{false};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
