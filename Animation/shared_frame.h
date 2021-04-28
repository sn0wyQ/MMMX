#ifndef ANIMATION_SHARED_FRAME_H_
#define ANIMATION_SHARED_FRAME_H_

#include <memory>
#include <unordered_map>

#include <QDebug>
#include <QFile>
#include <QString>
#include <QSvgRenderer>
#include <QTimer>

#include "Animation/animation_enums.h"
#include "constants.h"

const std::unordered_map<AnimationState, QString> kAnimationStateStrings {
    { AnimationState::kDestroy, "destroy" },
    { AnimationState::kIdle, "idle" },
    { AnimationState::kMove, "move" },
    { AnimationState::kShoot, "shoot" }
};

class SharedFrame {
 public:
  explicit SharedFrame(QString path,
                       AnimationState animation_state,
                       int frame_index);

  bool IsExists() const;
  int GetFrameIndex() const;
  std::shared_ptr<QSvgRenderer> GetSvgRenderer() const;

  static void UnloadUnusedFrames();

 private:
  static std::unordered_map<QString, std::shared_ptr<QSvgRenderer>>
      loaded_svgs_;
  static std::shared_ptr<QTimer> frames_unloader_;

  bool exists_ = false;
  int frame_index_;
  std::shared_ptr<QSvgRenderer>
      svg_renderer_ = std::make_shared<QSvgRenderer>();
};

#endif  // ANIMATION_SHARED_FRAME_H_
