#ifndef ANIMATIONS_HOLDER_H_
#define ANIMATIONS_HOLDER_H_

#include <memory>
#include <unordered_map>

#include "Animation/animation.h"
#include "Animation/animation_enums.h"

class AnimationsHolder {
 public:
  AnimationsHolder() = default;

  std::shared_ptr<Animation> GetAnimation(AnimationType animation_type);

  void UpdateAnimations(int delta_time);

 private:
  std::unordered_map<AnimationType, std::shared_ptr<Animation>> animations_;
};

#endif  // ANIMATIONS_HOLDER_H_
