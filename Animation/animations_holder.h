#ifndef ANIMATION_ANIMATIONS_HOLDER_H_
#define ANIMATION_ANIMATIONS_HOLDER_H_

#include <memory>
#include <unordered_map>

#include "Animation/animation.h"
#include "Animation/animation_enums.h"

const std::unordered_map<AnimationType, bool> kIsSynchronizedAllOfThisType {
    { AnimationType::kNone, true },
    { AnimationType::kTreeGreen, true }
};

class AnimationsHolder {
 public:
  AnimationsHolder() = default;

  std::shared_ptr<Animation> GetAnimation(AnimationType animation_type);

  void UpdateAnimations(int delta_time);

 private:
  std::unordered_multimap<AnimationType, std::shared_ptr<Animation>>
      animations_;
};

#endif  // ANIMATION_ANIMATIONS_HOLDER_H_
