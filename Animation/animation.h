#ifndef ANIMATION_ANIMATION_H_
#define ANIMATION_ANIMATION_H_

#include <exception>
#include <memory>
#include <queue>
#include <string>
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
    { AnimationType::kElectricGenerator, "ElectricGenerator" },
    { AnimationType::kGarage, "Garage" },
    { AnimationType::kGiantWeak, "GiantWeak" },
    { AnimationType::kGiantStrong, "GiantStrong" },
    { AnimationType::kBox, "Box" },
    { AnimationType::kSmasher, "Smasher" },
    { AnimationType::kSoldier, "Soldier" },
    { AnimationType::kSpider, "Spider" },
    { AnimationType::kTreeGreen, "TreeGreen" },
    { AnimationType::kViking, "Viking" },
    { AnimationType::kMap, "Map" },
    { AnimationType::kCrossbowBullet, "CrossbowBullet" },
    { AnimationType::kFilledBullet, "FilledBullet" }
};

const std::unordered_map<QString, AnimationState> kAnimationStateLabels {
    { "attack:", AnimationState::kAttack },
    { "idle:", AnimationState::kIdle },
    { "move:", AnimationState::kMove },
    { "shoot:", AnimationState::kShoot }
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

  void RenderFrame(Painter* painter, float w, float h);

  AnimationState GetState() const;
  AnimationType GetType() const;

  static void SetNumberOfFramesToPreloadInActiveSequence(int number_of_frames);
  static void SetNumberOfFramesToPreloadInInactiveSequences(
      int number_of_frames);

 private:
  void ParseAnimationDescription(const QString& description_path);

  static int frames_to_preload_in_active_sequence_;
  static int frames_to_preload_in_inactive_sequences_;

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
