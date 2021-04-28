#include "animations_holder.h"

std::shared_ptr<Animation>
    AnimationsHolder::GetAnimation(AnimationType animation_type) {
  if (kIsSynchronizedAllOfThisType.at(animation_type)) {
    auto iter = animations_.find(animation_type);
    if (iter == animations_.end()) {
      return animations_.insert({animation_type,
                                 std::make_shared<Animation>(animation_type)})
                                ->second;
    } else {
      return iter->second;
    }
  } else {
    return animations_.insert({animation_type,
                               std::make_shared<Animation>(animation_type)})
                                ->second;
  }
}

void AnimationsHolder::UpdateAnimations(int delta_time) {
  for (auto&[animation_type, animation] : animations_) {
    animation->Update(delta_time);
  }
}
