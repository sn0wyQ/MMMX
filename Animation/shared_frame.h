#ifndef ANIMATION_SHARED_FRAME_H_
#define ANIMATION_SHARED_FRAME_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QSvgRenderer>
#include <QTimer>

#include "Animation/animation_enums.h"
#include "Constants/constants.h"

const std::unordered_map<AnimationState, QString> kAnimationStateStrings {
    { AnimationState::kAttack, "attack" },
    { AnimationState::kDestroy, "destroy" },
    { AnimationState::kIdle, "idle" },
    { AnimationState::kMove, "move" },
    { AnimationState::kShoot, "shoot" }
};

class SharedFrame {
 public:
  explicit SharedFrame(const QString& path_prefix,
                       AnimationState animation_state,
                       int frame_index,
                       const QSize& predicted_size = QSize());

  bool IsExists() const;
  int GetFrameIndex() const;
  QSize GetPixmapSize() const;
  std::shared_ptr<QPixmap> GetRenderedPixmap(int w, int h);

  static void UnloadUnusedResources();
  static void UnloadUnusedPixmaps();
  static void UnloadUnusedSvgRenderers();

 private:
  static std::unordered_map<QString, std::shared_ptr<QSvgRenderer>>
      loaded_svgs_;
  static std::map<std::pair<QString, std::pair<int, int>>,
                  std::shared_ptr<QPixmap>> rendered_pixmaps_;
  static std::shared_ptr<QTimer> resource_unloader_;

  bool is_exists_{false};
  int frame_index_;
  QString path_;
  std::shared_ptr<QSvgRenderer>
      svg_renderer_ = std::make_shared<QSvgRenderer>();
  std::shared_ptr<QPixmap> pixmap_ = std::make_shared<QPixmap>();
};

#endif  // ANIMATION_SHARED_FRAME_H_
