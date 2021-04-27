#ifndef ANIMATION_ENUMS_H_
#define ANIMATION_ENUMS_H_

#include <QMetaObject>
#include <QMetaType>

namespace AnimationEnumsWrapper {

Q_NAMESPACE

enum class AnimationInstruction {
  // Sets |animation_state_| to AnimationState::kIdle
  // Sets |animation_frame_index_| to 0
  // Sets |animation_instruction_index_| to 0
  // no arguments
  kEnd,

  // Sets |animation_frame_index_| to 0
  // Sets |animation_instruction_index_| to 0
  // no arguments
  kLoop,

  // Increments |animation_frame_index_|
  // no arguments
  kNextFrame,

  // Alias for:
  //   nextframe
  //   wait %wait_time%
  // written %frames_number% times
  // [0] - frames_number
  // [1] - wait_time
  kPlayFrames,


  // Waits %wait_time% ms before next instruction
  // [0] - wait_time
  kWait,

  // Waits QRandomGenerator::global()->bounded(%min_wait_time%, %max_wait_time%)
  // ms before next instruction
  // [0] - min_wait_time
  // [1] - max_wait_time
  kWaitRand
};

Q_ENUM_NS(AnimationInstruction)

// Animation currently has 4 states:
// 1) kDestroy (after object is destroyed)
// 2) kIdle (when not moving or doing anything)
// 3) kMove (when only moving)
// 4) kShoot (when shooting and possibly moving)
enum class AnimationState {
  // First one MUST be equal to 0!!!
  // (so later we could easily iterate through all animation states)
  kDestroy = 0,
  kIdle,
  kMove,
  kShoot,

  SIZE
};

Q_ENUM_NS(AnimationState)

enum class AnimationType {
  // MUST stay -1, so first "real" animation type would be 0
  // Used for objects without images (that are just drawn for example)
  kNone = -1,

  kTreeGreen
};

Q_ENUM_NS(AnimationType)

}  // namespace AnimationEnumsWrapper

using AnimationInstruction = AnimationEnumsWrapper::AnimationInstruction;
Q_DECLARE_METATYPE(AnimationInstruction)

using AnimationState = AnimationEnumsWrapper::AnimationState;
Q_DECLARE_METATYPE(AnimationState)

using AnimationType = AnimationEnumsWrapper::AnimationType;
Q_DECLARE_METATYPE(AnimationType)

#endif  // ANIMATION_ENUMS_H_
