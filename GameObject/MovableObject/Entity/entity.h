#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_

#include <memory>
#include <utility>
#include <vector>

#include <QPainterPath>

#include "GUI/Animations/linear_emulator.h"
#include "GameObject/MovableObject/movable_object.h"

class Entity : public MovableObject {
 public:
  explicit Entity(GameObjectId id);
  Entity(const Entity& other);
  ~Entity() override = default;

  void OnTick(int delta_time) override;
  void TickHealthPoints(int delta_time);

  void DrawHealthBar(Painter* painter) const override;
  void DrawLevel(Painter* painter) const override;

  float GetFovRadius() const;
  void SetFovRadius(float fov_radius);

  void SetParams(std::vector<QVariant> params) override;
  std::vector<QVariant> GetParams() const override;

  void SetHealthPoints(float health_points);
  float GetHealthPoints() const;

  void SetMaxHealthPoints(float max_health_points);
  float GetMaxHealthPoints() const;
  bool IsAlive() const override;

  void SetLevel(int level);
  int GetLevel() const;

  bool IsEntity() const override;
  virtual bool IsPlayer() const;

  virtual void Revive(QPointF point_to_spawn);

  void SetHealthRegenRate(float health_regen_rate);
  float GetHealthRegenRate() const;

  virtual float GetExpIncrementForKill() const;

  std::shared_ptr<GameObject> Clone() const override;

  float GetOpacity() const override;
  void SetAppearing() override;
  void SetDisappearing() override;

  void UpdateAnimationState(bool restart = false) override;

 protected:
  int level_{1};

 private:
  void HideHealthPointBar() const;
  void ShowHealthPointBar() const;

  mutable int64_t last_changed_hp_{0};
  mutable float hp_bar_opacity_{0.f};
  float fov_radius_{Constants::kDefaultEntityFov};
  float health_points_{0.f};
  float health_regen_rate_{};
  float max_health_points_{};
  bool is_alive_{true};
  LinearEmulator<float> opacity_emulator_;
  mutable LinearEmulator<float> hp_bar_opacity_emulator_;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_ENTITY_H_
