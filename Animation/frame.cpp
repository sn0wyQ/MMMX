#include "frame.h"

QString CalcLeadingZeros(int x) {
  if (x < 10) {
    return "00";
  } else if (x < 100) {
    return "0";
  } else {
    return "";
  }
}

Frame::Frame(QString path,
             AnimationState animation_state,
             int frame_index)
    : frame_index_(frame_index) {
  path += animation_state_strings_.at(animation_state)
      + CalcLeadingZeros(frame_index_) + QString::number(frame_index_) + ".svg";

  if (!QFile::exists(path)) {
    return;
  }

  svg_renderer_->load(path);
  exists_ = true;
}

bool Frame::IsExists() const {
  return exists_;
}

int Frame::GetFrameIndex() const {
  return frame_index_;
}

std::shared_ptr<QSvgRenderer> Frame::GetSvgRenderer() const {
  return svg_renderer_;
}

