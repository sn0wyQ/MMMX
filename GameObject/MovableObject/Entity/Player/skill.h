#ifndef GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_SKILL_H_
#define GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_SKILL_H_

#include <QImage>

class Skill {
 public:
  Skill() = default;

 private:
  QImage image_;
  float cool_down_ = 0.f;
  float cool_down_time_left_ = 0.f;
};

#endif  // GAMEOBJECT_MOVABLEOBJECT_ENTITY_PLAYER_SKILL_H_
