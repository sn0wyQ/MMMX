#ifndef ANIMATION_ANIMATIONS_HOLDER_H_
#define ANIMATION_ANIMATIONS_HOLDER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "Animation/animation.h"
#include "Animation/animation_enums.h"
#include "constants.h"

const std::unordered_map<AnimationType, bool> kIsSynchronizedAllOfThisType {
    { AnimationType::kNone, true },
    { AnimationType::kBoxSciFiCube, true },
    { AnimationType::kBoxSciFiLong, true },
    { AnimationType::kJoCost, false },
    { AnimationType::kShenandoah, false },
    { AnimationType::kSpider3_0, false },
    { AnimationType::kTreeGreen, true },
    { AnimationType::kXaleKokhrap, false },
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
