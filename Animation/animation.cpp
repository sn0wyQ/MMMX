#include "animation.h"

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

    ParseAnimationDescription(base_path_ + "description");
  }
}

void Animation::ParseAnimationDescription(const QString& description_path) {
  if (!QFile::exists(description_path)) {
    return;
  }

  QFile description_file(description_path);
  int line_index = 1;
  if (description_file.open(QIODevice::ReadOnly)) {
    AnimationState animation_state = AnimationState::SIZE;
    while (description_file.bytesAvailable() > 0) {
      QString current_line = description_file.readLine();
      QStringList
          parts = current_line.split(QRegExp("[\n\r\t ]"), Qt::SkipEmptyParts);

      if (parts.empty() || parts[0].startsWith('#')) {
        // Just do nothing - empty line or line with comment
      } else if (auto label_iter = kAnimationStateLabels.find(parts[0]);
                 label_iter != kAnimationStateLabels.end()) {
        animation_state = label_iter->second;
      } else {
        if (animation_state == AnimationState::SIZE) {
          throw std::runtime_error(
              ("Invalid animation description format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        if (animation_instructions_.find(animation_state)
            == animation_instructions_.end()) {
          animation_instructions_.insert({animation_state, InstructionList()});
        }

        if (auto instruction_iter =
              kAnimationCommandsToInstructions.find(parts[0]);
            instruction_iter != kAnimationCommandsToInstructions.end()) {
          animation_instructions_.at(animation_state)
              .push_back({instruction_iter->second, {}});
          if (instruction_iter->second == AnimationInstructionType::kEnd
              || instruction_iter->second == AnimationInstructionType::kLoop) {
            animation_state = AnimationState::SIZE;
          }
        } else {
          throw std::runtime_error(
              ("Invalid animation instruction format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        // If we met '#' that means that comment started
        for (int index = 1;
             index < parts.size() && !parts[index].startsWith('#'); ++index) {
          animation_instructions_.at(animation_state)
              .back().args.push_back(parts[index].toInt());
        }
      }
      ++line_index;
    }
  } else {
    qWarning() << "Error while opening " << description_path;
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
        std::queue<SharedFrame>& frames = animation_frames_.at(animation_state_);

        // If sequence contains only one frame we don't touch anything
        // (We guarantee that if sequence has more than one frame frames.size()
        //  would return more, than one)
        if (frames.size() > 1) {
          frames.pop();
          SharedFrame next_frame
              (base_path_,
               animation_state_,
               frames.front().GetFrameIndex() + 1);
          if (next_frame.IsExists()) {
            frames.push(next_frame);
          } else {
            // if frames.front() frame is last in sequence
            frames.emplace(base_path_, animation_state_, 0);
          }
          if (!frames.front().IsExists()) {
            throw std::runtime_error(
                "Loaded non existing frame to final sequence");
          }
        }
        ++animation_instruction_index_;
        break;
      }

      case AnimationInstructionType::kPlayFrames: {
        InstructionList instructions_to_insert_;
        for (int index = 0;
              index < current_instruction_list.
                at(animation_instruction_index_).args.at(0);
              ++index) {
          instructions_to_insert_
            .push_back({AnimationInstructionType::kNextFrame, {}});
          instructions_to_insert_.push_back({AnimationInstructionType::kWait,
                                             {current_instruction_list.at(
                                                animation_instruction_index_)
                                                .args.at(1)}});
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

    if (animation_instruction_index_ >= current_instruction_list.size()) {
      throw std::runtime_error("Animation must be terminated "
                               "with either \"end\" or \"loop\"");
    }
  }
}

void Animation::SetAnimationState(AnimationState animation_state, bool forced) {
  if (!forced && animation_state_ == animation_state) {
    return;
  }

  // Clearing current animation state's frames queue
  // and adding there first frame of sequence
  std::queue<SharedFrame>().swap(animation_frames_.at(animation_state_));
  animation_frames_.at(animation_state_).emplace(base_path_,
                                                 animation_state_,
                                                 0);

  animation_state_ = animation_state;
  animation_instruction_index_ = 0;
  go_to_next_instruction_time_ = current_animation_time_;

  // We guarantee that we always already have 0th frame
  // (if it exists in storage) and we also want to preload next frame
  animation_frames_.at(animation_state_).emplace(base_path_,
                                                 animation_state_,
                                                 1);
}

void Animation::RenderFrame(Painter* painter, float w, float h) const {
  const SharedFrame&
      frame_to_render = animation_frames_.at(animation_state_).front();
  if (!frame_to_render.IsExists()) {
    return;
  }

  painter->RenderSvg(QPointF(),
                     w,
                     h,
                     frame_to_render.GetSvgRenderer());
}

AnimationType Animation::GetType() const {
  return animation_type_;
}
