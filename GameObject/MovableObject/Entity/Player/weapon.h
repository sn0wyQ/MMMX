#ifndef WEAPON_H_
#define WEAPON_H_

#include <QImage>

class Weapon {
 public:
  Weapon() = default;

 private:
  float base_damage_ = 0.f;
  float bullet_speed_ = 0.f;
  float rate_of_fire_ = 0.f;
  QImage image_;
  int current_clip_size_ = 0;
  float reloading_time_ = 0.f;
  float range_penalty_ = 0.f;
  int max_clip_size_ = 0;
};

#endif  // WEAPON_H_
