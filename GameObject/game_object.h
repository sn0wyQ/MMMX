#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <QObject>
#include <QPoint>

#include "Animation/animation.h"
#include "constants.h"

class GameObject : public QObject {
  Q_OBJECT
  Q_PROPERTY(int id MEMBER id_)
  Q_PROPERTY(QPoint position MEMBER position_)

 public:
  GameObject() = default;
  explicit GameObject(GameObjectId id);

  virtual void OnTick() = 0;

  GameObjectId GetId();
  void SetId(GameObjectId id);

 private:
  GameObjectId id_ = Constants::kNullGameObjectId;
  QPoint position_;
  Animation animation_;
};

#endif  // GAME_OBJECT_H_
