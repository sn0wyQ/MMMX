#include "animation.h"

int Animation::frames_to_preload_in_active_sequence_ = 5;
int Animation::frames_to_preload_in_other_sequences_ = 2;

Animation::Animation(AnimationType animation_type)
    : animation_type_(animation_type) {
  if (animation_type == AnimationType::kNone) {
    return;
  } else {
    base_path_ =
        "./Res/Animation/" + kAnimationTypeStrings.at(animation_type) + "/";

    // Loading first frames of sequence for each animation state
    int animation_state_index = 0;
    auto animation_state = static_cast<AnimationState>(0);
    while (animation_state < AnimationState::SIZE) {
      std::queue<SharedFrame> first_frames_of_state;
      first_frames_of_state.emplace(base_path_, animation_state, 0);
      if (first_frames_of_state.front().IsExists()) {
        if (animation_state == animation_state_) {
          first_frames_of_state.emplace(base_path_, animation_state, 1);
        }
        animation_frames_.insert({animation_state, first_frames_of_state});
      }
      ++animation_state_index;
      animation_state = static_cast<AnimationState>(animation_state_index);
    }

    ParseAnimationDescription(base_path_ + "description.mmmx");
  }
}

void Animation::ParseAnimationDescription(const QString& description_path) {
  if (!QFile::exists(description_path)) {
    throw std::runtime_error("[ANIMATION] Description file does not exist");
  }

  QFile description_file(description_path);
  int line_index = 1;
  if (description_file.open(QIODevice::ReadOnly)) {
    AnimationState animation_state = AnimationState::SIZE;
    while (description_file.bytesAvailable() > 0) {
      QString current_line = description_file.readLine();
      QStringList
          parts = current_line.split(QRegExp("[\n\r\t ]"), Qt::SkipEmptyParts);

      if (parts.empty() || parts.at(0).startsWith('#')) {
        // Just do nothing - empty line or line with comment
      } else if (auto label_iter = kAnimationStateLabels.find(parts.at(0));
                 label_iter != kAnimationStateLabels.end()) {
        animation_state = label_iter->second;
      } else {
        if (animation_state == AnimationState::SIZE) {
          throw std::runtime_error(
              ("[ANIMATION] Invalid description format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        if (animation_instructions_.find(animation_state)
            == animation_instructions_.end()) {
          animation_instructions_.insert({animation_state, InstructionList()});
        }

        auto instruction_iter = kAnimationCommandsToInstructions.find(
                                                                parts.at(0));
        if (instruction_iter != kAnimationCommandsToInstructions.end()) {
          animation_instructions_.at(animation_state)
              .push_back({instruction_iter->second, {}});
          if (instruction_iter->second == AnimationInstructionType::kEnd
              || instruction_iter->second == AnimationInstructionType::kLoop) {
            animation_state = AnimationState::SIZE;
          }
        } else {
          throw std::runtime_error(
              ("[ANIMATION] Invalid animation instruction format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        // If we met '#' that means that comment started
        for (int index = 1;
             index < parts.size() && !parts.at(index).startsWith('#');
             ++index) {
          animation_instructions_.at(animation_state)
              .back().args.push_back(parts.at(index).toInt());
        }
      }
      ++line_index;
    }
  } else {
    qWarning() << "[ANIMATION] Error while opening " << description_path;
  }
}

void Animation::Update(int delta_time) {
  if (animation_type_ == AnimationType::kNone) {
    return;
  }
  current_animation_time_ += delta_time;
  while (current_animation_time_ >= go_to_next_instruction_time_) {
    InstructionList&
        current_instruction_list = animation_instructions_.at(animation_state_);
    switch (current_instruction_list.at(animation_instruction_index_).type) {
      case AnimationInstructionType::kEnd: {
        SetAnimationState(AnimationState::kIdle, true);
        break;
      }

      case AnimationInstructionType::kLoop: {
        SetAnimationState(animation_state_, true);
        break;
      }

      case AnimationInstructionType::kNextFrame: {
        std::queue<SharedFrame>&
            frames = animation_frames_.at(animation_state_);

        // If sequence contains only one frame we don't touch anything
        // (We guarantee that if sequence has more than one frame frames.size()
        //  would return more, than one)
        if (frames.size() > 1) {
          frames.pop();
          while (static_cast<int>(frames.size())
                 < frames_to_preload_in_active_sequence_) {
            SharedFrame next_frame(base_path_,
                                   animation_state_,
                                   frames.back().GetFrameIndex() + 1,
                                   frames.back().GetPixmapSize());
            if (next_frame.IsExists()) {
              frames.push(next_frame);
            } else {
              // if frames.front() frame is last in sequence
              frames.emplace(base_path_, animation_state_, 0,
                             frames.back().GetPixmapSize());
            }
          }
          if (!frames.front().IsExists()) {
            throw std::runtime_error(
                "[ANIMATION] Loaded non existing frame to final sequence");
          }
        }
        ++animation_instruction_index_;
        break;
      }

      case AnimationInstructionType::kPlayFrames: {
        InstructionList instructions_to_insert_;
        int frames_count = current_instruction_list.
            at(animation_instruction_index_).args.at(0);
        for (int instruction_index = 0; instruction_index < frames_count;
             ++instruction_index) {
          instructions_to_insert_
              .push_back({AnimationInstructionType::kNextFrame, {}});
          int wait_time = current_instruction_list
              .at(animation_instruction_index_).args.at(1);
          instructions_to_insert_.push_back({AnimationInstructionType::kWait,
                                             {wait_time}});
        }
        current_instruction_list.erase(
            current_instruction_list.begin()
                + static_cast<int>(animation_instruction_index_));
        current_instruction_list.insert(
            current_instruction_list.begin()
                + static_cast<int>(animation_instruction_index_),
            instructions_to_insert_.begin(),
            instructions_to_insert_.end());
        break;
      }

      case AnimationInstructionType::kWait: {
        go_to_next_instruction_time_
            += current_instruction_list
                .at(animation_instruction_index_).args.at(0);
        ++animation_instruction_index_;
        break;
      }

      case AnimationInstructionType::kWaitRand: {
        int min_wait_time =
            current_instruction_list
                .at(animation_instruction_index_).args.at(0);
        int max_wait_time =
            current_instruction_list
                .at(animation_instruction_index_).args.at(1);
        go_to_next_instruction_time_ +=
            QRandomGenerator::global()->bounded(min_wait_time, max_wait_time);
        ++animation_instruction_index_;
        break;
      }
    }

    if (animation_instruction_index_
        >= static_cast<int>(current_instruction_list.size())) {
      throw std::runtime_error("[ANIMATION] Animation must be terminated "
                               "with either \"end\" or \"loop\"");
    }
  }
}

void Animation::SetAnimationState(AnimationState animation_state,
                                  bool restart) {
  std::queue<SharedFrame>&
      current_sequence = animation_frames_.at(animation_state_);

  if (animation_state_ == animation_state
      && (!restart || (!current_sequence.empty()
                       && current_sequence.front().GetFrameIndex() == 0))) {
    return;
  }

  // Clearing current animation state's frames queue
  // and adding there first frame of sequence
  QSize saved_size;
  if (!current_sequence.empty()) {
    saved_size = current_sequence.front().GetPixmapSize();
    current_sequence = {};
    current_sequence.emplace(base_path_, animation_state_, 0, saved_size);

    int current_frame_index = 1;
    while (current_sequence.back().IsExists()
           && current_frame_index < frames_to_preload_in_other_sequences_) {
      current_sequence.emplace(base_path_,
                               animation_state_,
                               current_frame_index++,
                               saved_size);
    }
  }

  animation_state_ = animation_state;
  current_sequence = animation_frames_.at(animation_state_);
  animation_instruction_index_ = 0;
  go_to_next_instruction_time_ = current_animation_time_;

  // We guarantee that we always already have 0th frame
  // (if it exists in storage) and we also want to preload more frames
  int current_frame_index = 1;
  while (current_sequence.back().IsExists()
         && current_frame_index < frames_to_preload_in_active_sequence_) {
    current_sequence.emplace(base_path_,
                             animation_state_,
                             current_frame_index++,
                             saved_size);
  }
}

void Animation::RenderFrame(Painter* painter, float w, float h) {
  if (animation_frames_.at(animation_state_).empty()) {
    return;
  }

  SharedFrame*
      frame_to_render = &animation_frames_.at(animation_state_).front();
  if (!frame_to_render->IsExists()) {
    return;
  }
  painter->DrawSharedFrame(QPointF(), w, h, frame_to_render);
}

AnimationType Animation::GetType() const {
  return animation_type_;
}

void Animation::SetNumberOfFramesToPreloadInActiveSequence(
    int number_of_frames) {
  assert(number_of_frames > 0
         && "[ANIMATION] Can not disable preloading current frame!");
  frames_to_preload_in_active_sequence_ = number_of_frames;
  if (frames_to_preload_in_active_sequence_
      < frames_to_preload_in_other_sequences_) {
    qWarning() << "[ANIMATION] It is highly recommended for"
                  "number of frames in active sequence to be NOT less than"
                  "number of frames in other sequences!";
  }
}
void Animation::SetNumberOfFramesToPreloadInOtherSequences(
    int number_of_frames) {
  frames_to_preload_in_other_sequences_ = number_of_frames;
  if (frames_to_preload_in_active_sequence_
      < frames_to_preload_in_other_sequences_) {
    qWarning() << "[ANIMATION] It is highly recommended for"
                  "number of frames in active sequence to be NOT less than"
                  "number of frames in other sequences!";
  }
}
