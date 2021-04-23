#ifndef PLAYER_BAR_H
#define PLAYER_BAR_H

#include <memory>

#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class PlayerBar : public QWidget {
  Q_OBJECT

 public:
  PlayerBar(QWidget* parent, std::shared_ptr<ClientGameModel>  model);

  void paintEvent(QPaintEvent* paint_event) override;

  QRectF RectWithPercents(float x1, float y1, float width, float height);
  QPointF PointWithPercents(float x, float y);

 private:
  std::shared_ptr<ClientGameModel> model_;
};

#endif //PLAYER_BAR_H
