#ifndef WEAPON_WEAPON_H_
#define WEAPON_WEAPON_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QDebug>

#include "GameObject/game_object.h"
#include "GameObject/MovableObject/Bullet/bullet.h"
#include "Weapon/WeaponConstants/weapon_constants.h"

enum class WeaponType {
  kAssaultRifle,
  kCrossbow,
  kMachineGun,
  kShotgun
};

class Weapon {
 public:
  Weapon() = default;
  Weapon(const Weapon& weapon) = default;

  void Reload(int64_t cur_time);
  bool IsPossibleToShoot(int64_t cur_time) const;
  void SetLastTimeShot(int64_t cur_time);
  int64_t GetTimeBetweenShoots() const;

  virtual void DrawWeapon(Painter* painter) = 0;

  void SetBulletDamage(float base_bullet_damage);
  float GetBulletDamage() const;

  void SetBulletSpeed(float bullet_speed);
  float GetBulletSpeed() const;

  void SetBulletRange(float bullet_range);
  float GetBulletRange() const;

  void SetRateOfFire(int rate_of_fire);
  int GetRateOfFire() const;

  int64_t GetReloadingTime() const;
  void SetReloadingTime(int64_t reloading_time);

  int GetClipSize() const;
  void SetClipSize(int clip_size);

  int GetCurrentBulletsInClip() const;
  void SetCurrentBulletsInClip(int current_bullets_in_clip);

  std::vector<QVariant> GetBulletParams(GameObjectId parent_id,
                                        float x,
                                        float y,
                                        float rotation,
                                        float radius) const;

  std::vector<std::vector<QVariant>> GetBulletsParams(
      GameObjectId parent_id,
      float x,
      float y,
      float rotation) const;

  virtual WeaponType GetWeaponType() const = 0;

 private:
  float bullet_damage_{};  // дамаг (может прокачать герой)
  float bullet_speed_{};  // скорость полета пули
  float bullet_range_{};  // расстояние полета пули
  int rate_of_fire_{};  // скорострельность пушки (кол-во выстрелов в минуту)
  int64_t reloading_time_{};  // время перезарядки (может прокачать герой)
  int clip_size_{};  // размер обоймы (может прокачать герой)
  int current_bullets_in_clip_{};  // текущее кол-во патронов в обойме

  int64_t last_time_shot_{};
  int64_t last_time_pressed_reload_{};
};
#endif  // WEAPON_WEAPON_H_
