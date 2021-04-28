#ifndef ANIMATION_FRAME_H_
#define ANIMATION_FRAME_H_

#include <memory>
#include <unordered_map>

#include <QDebug>
#include <QFile>
#include <QString>
#include <QSvgRenderer>

#include "Animation/animation_enums.h"

const std::unordered_map<AnimationState, QString> animation_state_strings_ {
    { AnimationState::kDestroy, "destroy" },
    { AnimationState::kIdle, "idle" },
    { AnimationState::kMove, "move" },
    { AnimationState::kShoot, "shoot" }
};

class Frame {
 public:
  explicit Frame(QString path,
                 AnimationState animation_state,
                 int frame_index);

  bool IsExists() const;
  int GetFrameIndex() const;
  std::shared_ptr<QSvgRenderer> GetSvgRenderer() const;

 private:
  bool exists_ = false;
  int frame_index_;
  std::shared_ptr<QSvgRenderer>
      svg_renderer_ = std::make_shared<QSvgRenderer>();
};

#endif  // ANIMATION_FRAME_H_
