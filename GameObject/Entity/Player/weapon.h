#ifndef WEAPON_H_
#define WEAPON_H_

#include <QImage>

class Weapon {
 public:
  Weapon() = default;

 private:
  float base_damage_;
  float bullet_speed_;
  float rate_of_fire_;
  QImage image_;
  int current_clip_size_;
  float reloading_time_;
  float range_penalty_;
  int max_clip_size_;
};

#endif  // WEAPON_H_
