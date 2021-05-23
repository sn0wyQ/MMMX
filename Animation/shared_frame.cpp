#include "shared_frame.h"

std::unordered_map<QString, std::shared_ptr<QSvgRenderer>>
    SharedFrame::loaded_svgs_;
std::map<std::pair<QString, std::pair<int, int>>, std::shared_ptr<QPixmap>>
    SharedFrame::rendered_pixmaps_;
std::shared_ptr<QTimer>
    SharedFrame::resource_unloader_ = std::make_shared<QTimer>();

QString CalcLeadingZeros(int x) {
  if (x < 10) {
    return "00";
  } else if (x < 100) {
    return "0";
  } else {
    return "";
  }
}

SharedFrame::SharedFrame(const QString& path_prefix,
                         AnimationState animation_state,
                         int frame_index,
                         const QSize& predicted_size)
    : frame_index_(frame_index) {
  path_ = path_prefix + kAnimationStateStrings.at(animation_state)
      + CalcLeadingZeros(frame_index_) + QString::number(frame_index_)
      + ".svg";

  if (!QFile::exists(path_)) {
    return;
  } else {
    is_exists_ = true;
  }

#ifndef MMMX_SERVER
  if (!resource_unloader_->isActive()) {
    resource_unloader_->start(Constants::kUnloadAnimationCheckTime);
    resource_unloader_->callOnTimeout(SharedFrame::UnloadUnusedResources);
  }

  auto svg_iter = loaded_svgs_.find(path_);
  if (svg_iter == loaded_svgs_.end()) {
    svg_renderer_ = std::make_shared<QSvgRenderer>(path_);
    loaded_svgs_[path_] = svg_renderer_;
  } else {
    svg_renderer_ = svg_iter->second;
  }

  if (predicted_size.isValid()) {
    GetRenderedPixmap(predicted_size.width(), predicted_size.height());
  }
#endif  // MMMX_SERVER
}

bool SharedFrame::IsExists() const {
  return is_exists_;
}

int SharedFrame::GetFrameIndex() const {
  return frame_index_;
}

QSize SharedFrame::GetPixmapSize() const {
  return (pixmap_->isNull() ? QSize() : pixmap_->size());
}

std::shared_ptr<QPixmap> SharedFrame::GetRenderedPixmap(int w, int h) {
#ifndef MMMX_SERVER
  if (pixmap_->isNull() || pixmap_->size() != QSize(w, h)) {
    auto iter = rendered_pixmaps_.find({path_, {w, h}});
    if (iter != rendered_pixmaps_.end()) {
      pixmap_ = iter->second;
    } else {
      pixmap_ = std::make_shared<QPixmap>(w, h);
      pixmap_->fill(Qt::transparent);
      QPainter painter(pixmap_.get());
      svg_renderer_->render(&painter, pixmap_->rect());
      rendered_pixmaps_[{path_, {w, h}}] = pixmap_;
    }
  }
#endif  // MMMX_SERVER

  return pixmap_;
}

void SharedFrame::UnloadUnusedResources() {
  UnloadUnusedPixmaps();
  UnloadUnusedSvgRenderers();
}

void SharedFrame::UnloadUnusedPixmaps() {
  std::vector<QString> svg_renderers_to_delete;

  for (const auto&[path, loaded_svg] : loaded_svgs_) {
    if (loaded_svg.unique()) {
      svg_renderers_to_delete.push_back(path);
    }
  }

  for (const QString& frame_to_delete : svg_renderers_to_delete) {
    loaded_svgs_.erase(frame_to_delete);
  }
}

void SharedFrame::UnloadUnusedSvgRenderers() {
  std::vector<std::pair<QString, std::pair<int, int>>> pixmaps_to_delete;

  for (const auto&[path, rendered_pixmap] : rendered_pixmaps_) {
    if (rendered_pixmap.unique()) {
      pixmaps_to_delete.push_back(path);
    }
  }

  for (const auto& frame_to_delete : pixmaps_to_delete) {
    rendered_pixmaps_.erase(frame_to_delete);
  }
}
