#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <QObject>
#include <QPoint>

#include "Animation/animation.h"

class GameObject : public QObject {
  Q_OBJECT
  Q_PROPERTY(int id MEMBER id_)
  Q_PROPERTY(QPoint position MEMBER position_)

 public:
  GameObject() = default;

  virtual void OnTick() = 0;


 private:
  int id_;
  QPoint position_;
  Animation animation_;
};

#endif  // GAME_OBJECT_H_
