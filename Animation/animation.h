#ifndef ANIMATION_ANIMATION_H_
#define ANIMATION_ANIMATION_H_

#include <exception>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QMetaType>
#include <QPixmap>
#include <QRandomGenerator>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QtSvg/QtSvg>

#include "Painter/painter.h"

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
  // 0 - frames_number
  // 1 - wait_time
  kPlayFrames,


  // Waits %wait_time% ms before next instruction
  // 0 - wait_time
  kWait,

  // Waits QRandomGenerator::global()->bounded(%min_wait_time%, %max_wait_time%)
  // ms before next instruction
  // 0 - min_wait_time
  // 1 - max_wait_time
  kWaitRand
};

Q_ENUM_NS(AnimationInstruction)

// Animation has X (which is currently 3) states:
// 1) idle (when not moving or doing anything)
// 2) move (when only moving)
// 3) shoot (when shooting and possibly moving)
enum class AnimationState {
  // MUST stay -1, so first "real" animation state would be 0
  // Used for animations with AnimationType::kNone
  kNone = -1,

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

  kTreeGreen,

  SIZE
};

Q_ENUM_NS(AnimationType)

}  // namespace AnimationEnumsWrapper

using AnimationInstruction = AnimationEnumsWrapper::AnimationInstruction;
Q_DECLARE_METATYPE(AnimationInstruction)

using AnimationState = AnimationEnumsWrapper::AnimationState;
Q_DECLARE_METATYPE(AnimationState)

using AnimationType = AnimationEnumsWrapper::AnimationType;
Q_DECLARE_METATYPE(AnimationType)

using InstructionList = std::vector<std::pair<AnimationInstruction,
                                              std::vector<int>>>;

// Images for animations must lay in the Resources/Animation/%sprite_name%
class Animation {
 public:
  Animation() = default;
  explicit Animation(AnimationType animation_type);

  void Update(int delta_time);
  void SetAnimationState(AnimationState animation_state, bool forced = false);

  void RenderFrame(Painter* painter, float w, float h) const;

  AnimationType GetType() const;

 private:
  void ParseAnimation();
  void ParseAnimationDescription(const QString& description_path);

  // These are static so we parse each type of animation not more than one time.
  // Even if we have multiple objects using same AnimationType,
  // or if object was deleted and then restored
  static std::vector<std::vector<std::vector<std::shared_ptr<QSvgRenderer>>>>
      global_animation_frames_;
  static std::vector<std::vector<InstructionList>>
      global_animation_instructions_;

  AnimationState animation_state_ = AnimationState::kIdle;
  AnimationType animation_type_ = AnimationType::kNone;

  size_t animation_frame_index_ = 0;
  std::vector<std::vector<std::shared_ptr<QSvgRenderer>>>*
      animation_frames_ = nullptr;

  size_t animation_instruction_index_ = 0;
  std::vector<InstructionList>* animation_instructions_ = nullptr;

  size_t current_animation_time_ = 0;
  size_t go_to_next_instruction_time_ = 0;
};

#endif  // ANIMATION_ANIMATION_H_
