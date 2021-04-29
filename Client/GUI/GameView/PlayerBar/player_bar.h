#ifndef GUI_GAMEVIEW_PLAYER_BAR_H_
#define GUI_GAMEVIEW_PLAYER_BAR_H_

#include <memory>
#include <utility>

#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class PlayerBar : public QWidget {
  Q_OBJECT

 public:
  PlayerBar(QWidget* parent, std::shared_ptr<ClientGameModel>  model);

  void paintEvent(QPaintEvent* paint_event) override;

  void DrawNickname(QPainter* painter);
  void DrawHealthRect(QPainter* painter);
  void DrawExpRect(QPainter* painter);

  QRectF RectWithPercents(float x1, float y1, float width, float height);

 private:
  std::shared_ptr<ClientGameModel> model_;
};

#endif  // GUI_GAMEVIEW_PLAYER_BAR_H_
