#include "animation.h"

std::vector<std::vector<std::vector<std::shared_ptr<QSvgRenderer>>>>
    Animation::global_animation_frames_(static_cast<int>(AnimationType::SIZE));
std::vector<std::vector<InstructionList>>
    Animation::global_animation_instructions_
        (static_cast<int>(AnimationType::SIZE));

Animation::Animation(AnimationType animation_type)
    : animation_type_(animation_type) {
  if (animation_type == AnimationType::kNone) {
    return;
  }

  animation_frames_ =
      &global_animation_frames_.at(static_cast<int>(animation_type_));
  animation_frames_->resize(static_cast<int>(AnimationState::SIZE));

  animation_instructions_ =
      &global_animation_instructions_.at(static_cast<int>(animation_type_));
  animation_instructions_->resize(static_cast<int>(AnimationState::SIZE));

  // ParseAnimation only if we never parsed |animation_type| before
  if (animation_frames_->at(0).empty()
      && animation_instructions_->at(0).empty()) {
    ParseAnimation();
  }
}

QString CalcLeadingZeros(int x) {
  if (x < 10) {
    return "00";
  } else if (x < 100) {
    return "0";
  } else {
    return "";
  }
}

void Animation::ParseAnimation() {
  QString animation_name;
  switch (animation_type_) {
    case AnimationType::kTreeGreen:
      animation_name = "TreeGreen";
      break;

    default:
      return;
  }
  QString base_path = ":/" + animation_name + "/";

  // Read all files related to the sprite
  std::vector<QString> animation_states_strings = {"idle", "move", "shoot"};
  for (int animation_state_index = 0;
       animation_state_index < static_cast<int>(AnimationState::SIZE);
       ++animation_state_index) {
    QString current_path_prefix =
        base_path + animation_states_strings.at(animation_state_index);
    int image_index = 0;
    QString full_path;
    while (full_path = current_path_prefix + CalcLeadingZeros(image_index)
        + QString::number(image_index) + ".svg", QFile::exists(full_path)) {
      animation_frames_->at(animation_state_index).push_back(
          std::make_shared<QSvgRenderer>(full_path));
      ++image_index;
    }
  }

  // Parse description file
  ParseAnimationDescription(base_path + "description");
}

void Animation::ParseAnimationDescription(const QString& description_path) {
  if (!QFile::exists(description_path)) {
    return;
  }

  QFile description_file(description_path);
  int line_index = 1;
  if (description_file.open(QIODevice::ReadOnly)) {
    AnimationState animation_state_index = AnimationState::kNone;
    while (description_file.bytesAvailable() > 0) {
      QString current_line = description_file.readLine();
      QStringList
          parts = current_line.split(QRegExp("[\n\r\t ]"), Qt::SkipEmptyParts);

      if (parts.empty() || parts[0].startsWith('#')) {
        // Just do nothing - empty line or line with comment
      } else if (parts[0] == "idle:") {
        animation_state_index = AnimationState::kIdle;
      } else if (parts[0] == "move:") {
        animation_state_index = AnimationState::kMove;
      } else if (parts[0] == "shoot:") {
        animation_state_index = AnimationState::kShoot;
      } else {
        if (animation_state_index == AnimationState::kNone) {
          throw std::runtime_error(
              ("Invalid animation description format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        if (parts[0] == "end") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
            .push_back({AnimationInstruction::kEnd, {}});
          animation_state_index = AnimationState::kNone;
        } else if (parts[0] == "loop") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .push_back({AnimationInstruction::kLoop, {}});
          animation_state_index = AnimationState::kNone;
        } else if (parts[0] == "nextframe") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .push_back({AnimationInstruction::kNextFrame, {}});
        } else if (parts[0] == "playframes") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .push_back({AnimationInstruction::kPlayFrames, {}});
        } else if (parts[0] == "wait") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .push_back({AnimationInstruction::kWait, {}});
        } else if (parts[0] == "waitrand") {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .push_back({AnimationInstruction::kWaitRand, {}});
        } else {
          throw std::runtime_error(
              ("Invalid animation instruction format in "
                  + description_path
                  + ":" + QString::number(line_index)).toStdString());
        }

        // If we met '#' that means that comment started
        for (int index = 1;
             index < parts.size() && !parts[index].startsWith('#'); ++index) {
          animation_instructions_->at(static_cast<int>(animation_state_index))
              .back().second.push_back(parts[index].toInt());
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
    InstructionList& current_instruction_list =
        animation_instructions_->at(static_cast<int>(animation_state_));

    switch (current_instruction_list.at(animation_instruction_index_).first) {
      case AnimationInstruction::kEnd: {
        SetAnimationState(AnimationState::kIdle, true);
        break;
      }

      case AnimationInstruction::kLoop: {
        SetAnimationState(animation_state_, true);
        break;
      }

      case AnimationInstruction::kNextFrame: {
        ++animation_frame_index_;
        if (animation_frame_index_
            >= animation_frames_
                ->at(static_cast<int>(animation_state_)).size()) {
          throw std::runtime_error("Trying to play non-existing frame");
        }
        ++animation_instruction_index_;
        break;
      }

      case AnimationInstruction::kPlayFrames: {
        InstructionList instructions_to_insert_;
        for (int index = 0;
              index < current_instruction_list.
                at(animation_instruction_index_).second.at(0);
              ++index) {
          instructions_to_insert_.push_back({AnimationInstruction::kNextFrame,
                                             {}});
          instructions_to_insert_.push_back({AnimationInstruction::kWait,
                                             {current_instruction_list.at(
                                                animation_instruction_index_)
                                                .second.at(1)}});
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

      case AnimationInstruction::kWait: {
        go_to_next_instruction_time_
            += current_instruction_list
                .at(animation_instruction_index_).second.at(0);
        ++animation_instruction_index_;
        break;
      }

      case AnimationInstruction::kWaitRand: {
        int min_wait_time =
            current_instruction_list
                .at(animation_instruction_index_).second.at(0);
        int max_wait_time =
            current_instruction_list
                .at(animation_instruction_index_).second.at(1);
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

  animation_state_ = animation_state;
  animation_frame_index_ = 0;
  animation_instruction_index_ = 0;
  go_to_next_instruction_time_ = current_animation_time_;
}

void Animation::RenderFrame(Painter* painter, float w, float h) const {
  painter->RenderSvg(QPointF(),
                     w,
                     h,
                     animation_frames_->at(static_cast<int>(animation_state_))
                         .at(animation_frame_index_));
}

AnimationType Animation::GetType() const {
  return animation_type_;
}
