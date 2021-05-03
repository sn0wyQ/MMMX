#ifndef ANIMATION_ANIMATION_H_
#define ANIMATION_ANIMATION_H_

#include <exception>
#include <memory>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QDebug>
#include <QFile>
#include <QRandomGenerator>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "Animation/animation_enums.h"
#include "Animation/shared_frame.h"
#include "Painter/painter.h"

const std::unordered_map<AnimationType, QString> kAnimationTypeStrings {
    { AnimationType::kJoCost, "JoCost" },
    { AnimationType::kTreeGreen, "TreeGreen" }
};

const std::unordered_map<QString, AnimationState> kAnimationStateLabels {
    { "destroy:", AnimationState::kDestroy },
    { "idle:", AnimationState::kIdle },
    { "move:", AnimationState::kMove },
    { "shoot_to_idle:", AnimationState::kShootToIdle },
    { "shoot_to_move:", AnimationState::kShootToMove }
};

const std::unordered_map<QString, AnimationInstructionType>
    kAnimationCommandsToInstructions {
    { "end", AnimationInstructionType::kEnd },
    { "loop", AnimationInstructionType::kLoop },
    { "next_frame", AnimationInstructionType::kNextFrame },
    { "play_frames", AnimationInstructionType::kPlayFrames },
    { "wait", AnimationInstructionType::kWait },
    { "wait_rand", AnimationInstructionType::kWaitRand }
};

struct Instruction {
  AnimationInstructionType type;
  std::vector<int> args;
};
using InstructionList = std::vector<Instruction>;

// Images and descriptions for animations must lay in the
// Res/Animation/%sprite_name%
class Animation {
 public:
  explicit Animation(AnimationType animation_type);

  void Update(int delta_time);
  void SetAnimationState(AnimationState animation_state, bool restart = false);

  void RenderFrame(Painter* painter, float w, float h) const;

  AnimationType GetType() const;

 private:
  void ParseAnimationDescription(const QString& description_path);

  QString base_path_;

  AnimationType animation_type_ = AnimationType::kNone;
  AnimationState animation_state_ = AnimationState::kIdle;

  std::unordered_map<AnimationState, std::queue<SharedFrame>> animation_frames_;

  int animation_instruction_index_ = 0;
  std::unordered_map<AnimationState, InstructionList> animation_instructions_;

  int64_t current_animation_time_ = 0;
  int64_t go_to_next_instruction_time_ = 0;
};

#endif  // ANIMATION_ANIMATION_H_
