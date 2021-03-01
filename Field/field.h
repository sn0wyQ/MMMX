#ifndef FIELD_FIELD_H_
#define FIELD_FIELD_H_

#include <QImage>

#include "GameObject/game_object.h"

class Field {
 public:
  Field() = default;

 private:
  float width_ = 0.f;
  float height_ = 0.f;
  QImage bg_image_;
};

#endif  // FIELD_FIELD_H_
