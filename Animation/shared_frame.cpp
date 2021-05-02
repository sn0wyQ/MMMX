#include "shared_frame.h"

std::unordered_map<QString, std::shared_ptr<QSvgRenderer>>
    SharedFrame::loaded_svgs_;
std::shared_ptr<QTimer>
    SharedFrame::frames_unloader_ = std::make_shared<QTimer>();

QString CalcLeadingZeros(int x) {
  if (x < 10) {
    return "00";
  } else if (x < 100) {
    return "0";
  } else {
    return "";
  }
}

SharedFrame::SharedFrame(QString path,
                         AnimationState animation_state,
                         int frame_index)
    : frame_index_(frame_index) {
  path += kAnimationStateStrings.at(animation_state)
      + CalcLeadingZeros(frame_index_) + QString::number(frame_index_) + ".svg";

  if (!QFile::exists(path)) {
    return;
  } else {
    is_exists_ = true;
  }

  if (!frames_unloader_->isActive()) {
    frames_unloader_
      ->start(Constants::kUnloadAnimationCheckTime);
    frames_unloader_->callOnTimeout(SharedFrame::UnloadUnusedFrames);
  }

  auto iter = loaded_svgs_.find(path);
  if (iter == loaded_svgs_.end()) {
    auto new_renderer = std::make_shared<QSvgRenderer>(path);
    svg_renderer_ = loaded_svgs_[path] = new_renderer;
  } else {
    svg_renderer_ = iter->second;
  }
}

bool SharedFrame::IsExists() const {
  return is_exists_;
}

int SharedFrame::GetFrameIndex() const {
  return frame_index_;
}

std::shared_ptr<QSvgRenderer> SharedFrame::GetSvgRenderer() const {
  return svg_renderer_;
}

void SharedFrame::UnloadUnusedFrames() {
  std::vector<QString> frames_to_delete;

  for (const auto&[path, loaded_svg] : loaded_svgs_) {
    if (loaded_svg.unique()) {
      frames_to_delete.push_back(path);
    }
  }

  for (const QString& frame_to_delete : frames_to_delete) {
    loaded_svgs_.erase(frame_to_delete);
  }
}
