#include "animations_holder.h"

std::shared_ptr<Animation>
    AnimationsHolder::GetAnimation(AnimationType animation_type) {
  if (kIsSynchronizedAllOfThisType.at(animation_type)) {
    auto iter = animations_.find(animation_type);
    if (iter == animations_.end()) {
      auto new_animation = std::make_shared<Animation>(animation_type);
      animations_.insert({animation_type, new_animation});
      return new_animation;
    } else {
      return iter->second;
    }
  } else {
    auto new_animation = std::make_shared<Animation>(animation_type);
    animations_.insert({animation_type, new_animation});
    return new_animation;
  }
}

void AnimationsHolder::UpdateAnimations(int delta_time) {
  for (auto&[animation_type, animation] : animations_) {
    animation->Update(delta_time);
  }
  this->UnloadUnusedAnimations();
}

void AnimationsHolder::UnloadUnusedAnimations() {
  std::unordered_multimap<AnimationType, std::shared_ptr<Animation>>::iterator
      *prev_iter = nullptr;
  for (auto iter = animations_.begin(); iter != animations_.end(); ++iter) {
    if (iter->second.unique()) {
      animations_.erase(iter);
      if (prev_iter) {
        iter = *prev_iter;
      } else {
        iter = animations_.begin();
      }
    } else {
      prev_iter = &iter;
    }
  }
}
