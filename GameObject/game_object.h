#ifndef GAMEOBJECT_GAME_OBJECT_H_
#define GAMEOBJECT_GAME_OBJECT_H_

#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include <QDebug>
#include <QPoint>
#include <QString>

#include "Animation/animation.h"
#include "Animation/animations_holder.h"
#include "Painter/painter.h"
#include "Math/math.h"
#include "GameObject/RigidBody/intersect_constants.h"
#include "GameObject/RigidBody/rigid_body.h"
#include "GameObject/RigidBody/rigid_body_circle.h"
#include "GameObject/RigidBody/rigid_body_rectangle.h"
#include "Constants/constants.h"

namespace GameObjectTypeWrapper {

Q_NAMESPACE

// enum for sending events
// MUST be sorted in alphabet order
enum class GameObjectType {
  kBullet,
  kCreep,
  kGameObject,
  kMapBorder,
  kPlayer
};

Q_ENUM_NS(GameObjectType)

}  // namespace GameObjectTypeWrapper

using GameObjectType = GameObjectTypeWrapper::GameObjectType;
Q_DECLARE_METATYPE(GameObjectType)

class GameObject {
 public:
  explicit GameObject(GameObjectId id);
  GameObject(const GameObject& other);

  virtual void OnTick(int delta_time) {}
  void Draw(Painter* painter);
  virtual void DrawHealthBar(Painter* painter) {}
  virtual void DrawLevel(Painter* painter) {}
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
  QRectF GetBoundingRect() const;

  virtual GameObjectType GetType() const;

  virtual void SetParams(std::vector<QVariant> params);
  virtual std::vector<QVariant> GetParams() const;

  bool IsNeedToDelete() const;
  void SetIsNeedToDelete(bool is_need_to_delete);

  virtual bool IsFilteredByFov() const;

  std::shared_ptr<Animation> GetAnimation();
  void SetAnimation(AnimationType animation_type);
  static AnimationsHolder& GetAnimationsHolder();

  virtual bool IsEntity() const;

  virtual std::shared_ptr<GameObject> Clone() const;

  void SetUpdatedTime(int64_t updated_time);
  int64_t GetUpdatedTime() const;

  float GetRigidBodyBoundingCircleRadius() const;

  void SetCreatedTime(int64_t created_time);
  int64_t GetCreatedTime() const;

  void SetIsInterpolatedOnce(bool is_interpolated_once);
  bool IsInterpolatedOnce() const;

  bool IsVisible() const;
  void SetIsVisible(bool visible);

  void SetVisibility(float visibility);
  float GetVisibility() const;

  virtual bool IsAlive() const;

  bool IsNeedToDraw() const;

 private:
  // Holds animations for all GameObjects
  // Prevents same SharedFrame being loaded into RAM more than once at a time
  static AnimationsHolder animations_holder_;
  GameObjectId id_{Constants::kNullGameObjectId};
  std::shared_ptr<Animation>
      animation_ = std::make_shared<Animation>(AnimationType::kNone);
  QPointF position_{0.f, 0.f};
  // 0 is direction from left to right
  // Increasing counterclockwise
  // Measured in degrees
  float rotation_{0.f};
  float width_{0.f};
  float height_{0.f};
  std::shared_ptr<RigidBody> rigid_body_;
  bool is_need_to_delete_{false};
  int64_t updated_time_{};
  float visibility_{1.f};
  int64_t created_time_{};
  // Если мы только привязали объект к модели из интерполятора -
  // не можем его правильно проинтерполировать
  // мы не должны его рисовать
  bool is_interpolated_once_{false};
};

#endif  // GAMEOBJECT_GAME_OBJECT_H_
