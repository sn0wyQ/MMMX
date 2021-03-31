#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <vector>
#include <utility>
#include <memory>

#include <QPoint>
#include <QDebug>

#include "constants.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body.h"
#include "Painter/painter.h"

namespace GameObjectTypeWrapper {

Q_NAMESPACE

// enum for sending events
enum class GameObjectType {
  kPlayer,
  kBox,
  kTree
};

Q_ENUM_NS(GameObjectType)

}  // namespace GameObjectTypeWrapper

using GameObjectType = GameObjectTypeWrapper::GameObjectType;
Q_DECLARE_METATYPE(GameObjectType)

class GameObject {
 public:
  explicit GameObject(GameObjectId id, std::shared_ptr<RigidBody> rigid_body);
  GameObject(GameObjectId id, float x, float y, float rotation, float width,
             float height, std::shared_ptr<RigidBody> rigid_body);

  virtual void OnTick(int delta_time) = 0;
  void Draw(Painter* painter);
  virtual void DrawRelatively(Painter* painter) = 0;
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

  virtual GameObjectType GetType() const = 0;
  virtual void SetParams(const std::vector<QVariant>& params) = 0;

  virtual std::vector<QVariant> GetParams() const = 0;

  bool IsInFov() const;
  void SetIsInFov(bool is_in_fov);

 private:
  GameObjectId id_{Constants::kNullGameObjectId};
  QPointF position_{0.f, 0.f};
  float rotation_{0.f};
  float width_{0.f};
  float height_{0.f};
  std::shared_ptr<RigidBody> rigid_body_;
  bool is_in_fov_{true};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
