#ifndef ANIMATION_ANIMATIONS_HOLDER_H_
#define ANIMATION_ANIMATIONS_HOLDER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "Animation/animation.h"
#include "Animation/animation_enums.h"
#include "Constants/constants.h"

const std::unordered_map<AnimationType, bool> kIsSynchronizedAllOfThisType {
    { AnimationType::kNone, true },
    { AnimationType::kElectricGenerator, true },
    { AnimationType::kGarage, true },
    { AnimationType::kGiantWeak, false },
    { AnimationType::kGiantStrong, false },
    { AnimationType::kBox, true },
    { AnimationType::kSmasher, false },
    { AnimationType::kSoldier, false },
    { AnimationType::kSpider, false },
    { AnimationType::kTreeGreen, true },
    { AnimationType::kViking, false },
};

class AnimationsHolder {
 public:
  AnimationsHolder() = default;

  std::shared_ptr<Animation> GetAnimation(AnimationType animation_type);

  void UpdateAnimations(int delta_time);

  void UnloadUnusedAnimations();

 private:
  std::unordered_multimap<AnimationType, std::shared_ptr<Animation>>
      animations_;
};

#endif  // ANIMATION_ANIMATIONS_HOLDER_H_
