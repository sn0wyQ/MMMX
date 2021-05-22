#include "animations_holder.h"

std::shared_ptr<Animation>
    AnimationsHolder::GetAnimation(AnimationType animation_type) {
  auto sync_iter = kIsSynchronizedAllOfThisType.find(animation_type);
  if (sync_iter != kIsSynchronizedAllOfThisType.end() && sync_iter->second) {
    auto iter = animations_.find(animation_type);
    if (iter == animations_.end()) {
      auto new_animation = std::make_shared<Animation>(animation_type);
      animations_.insert({animation_type, new_animation});
      return new_animation;
    } else {
      return iter->second;
    }
  } else {
    if (sync_iter == kIsSynchronizedAllOfThisType.end()) {
      qWarning() << "[ANIMATION] Animation type" << animation_type
                 << "is not added to kIsSynchronizedAllOfThisType!";
    }

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
  std::vector<std::unordered_multimap<AnimationType,
                    std::shared_ptr<Animation>>::iterator> animations_to_delete;

  for (auto iter = animations_.begin(); iter != animations_.end(); ++iter) {
    if (iter->second.unique()) {
      animations_to_delete.push_back(iter);
    }
  }

  for (const auto& animation_to_delete : animations_to_delete) {
    animations_.erase(animation_to_delete);
  }
}
