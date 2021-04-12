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

class Weapon {
 public:

  void Shoot();
  void Reload();
  bool IsPossibleToShoot();
  void CreateBulletAndAddToModel();

  virtual void DrawWeapon(Painter* painter);

  float GetBaseBulletDamage() const;
  void SetBaseBulletDamage(float base_bullet_damage);
  float GetCurrentBulletDamage() const;
  void SetCurrentBulletDamage(float current_bullet_damage);
  void SetBulletSpeed(float bullet_speed);
  float GetBulletSpeed() const;
  void SetBulletRange(float bullet_range);
  float GetBulletRange() const;
  int GetRateOfFire() const;
  void SetRateOfFire(int rate_of_fire);
  float GetBaseReloadingTime() const;
  void SetBaseReloadingTime(float base_reloading_time);
  float GetCurrentReloadingTime() const;
  void SetCurrentReloadingTime(float current_reloading_time);
  int GetBaseClipSize() const;
  void SetBaseClipSize(int base_clip_size);
  int GetCurrentClipSize() const;
  void SetCurrentClipSize(int current_clip_size);
  float GetCurrentTimeToShoot() const;
  void SetCurrentTimeToShoot(float current_time_to_shoot);
  bool GetIsReloadingNow() const;
  void SetIsReloadingNow(bool is_reload_now);

 private:
  float base_bullet_damage_{}; // базовый дамаг(который может прокачать герой)
  float current_bullet_damage_{}; // текущий дамаг под всеми бонусами
  float bullet_speed_{}; // скорость полета пули
  float bullet_range_{}; // расстояние полета пули
  int rate_of_fire_{}; // скорострельность пушки (кол-во выстрелов в минуту)
  float base_reloading_time_{}; // базовое время перезарядки(который может прокачать герой)
  float current_reloading_time_{}; // текущее время до конца перезарядки
  int base_clip_size_{}; // базовый размер обоймы(который может прокачать герой)
  int current_clip_size_{}; // текущее кол-во патронов в обойме
  float current_time_to_shoot_{}; // время до следующего выстрела

  bool is_reloading_now_{false}; // перезаряжаемся сейчас?

};
#endif  // WEAPON_WEAPON_H_
