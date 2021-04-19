#ifndef WEAPON_WEAPON_H_
#define WEAPON_WEAPON_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <QDebug>
#include <QTimer>

#include "GameObject/game_object.h"
#include "GameObject/MovableObject/Bullet/bullet.h"
#include "Weapon/WeaponConstants/weapon_constants.h"
#include "Model/game_model.h"

enum class WeaponType {
  kMachineGun
};

class Weapon {
 public:
  void Reload(int64_t cur_time);
  bool IsPossibleToShoot(int64_t cur_time);
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

  float GetReloadingTime() const;
  void SetReloadingTime(float base_reloading_time);

  int GetClipSize() const;
  void SetClipSize(int clip_size);

  int GetCurrentBulletsInClip() const;
  void SetCurrentBulletsInClip(int current_bullets_in_clip);

 private:
  float bullet_damage_{}; // базовый дамаг(который может прокачать герой)
  float bullet_speed_{}; // скорость полета пули
  float bullet_range_{}; // расстояние полета пули
  int rate_of_fire_{}; // скорострельность пушки (кол-во выстрелов в минуту)
  float reloading_time_{}; // базовое время перезарядки(который может прокачать герой)
  int clip_size_{}; // базовый размер обоймы(который может прокачать герой)
  int current_bullets_in_clip_{}; // текущее кол-во патронов в обойме

  int64_t last_time_shooted_{};
  int64_t last_time_pressed_reload_{};
};
#endif  // WEAPON_WEAPON_H_
