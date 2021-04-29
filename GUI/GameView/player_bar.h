#ifndef GUI_GAMEVIEW_PLAYER_BAR_H_
#define GUI_GAMEVIEW_PLAYER_BAR_H_

#include <memory>
#include <utility>
#include <vector>

#include <QPaintEvent>
#include <QWidget>
#include <QPushButton>
#include <QGraphicsOpacityEffect>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class PlayerBar : public QWidget {
  Q_OBJECT

 public:
  PlayerBar(QWidget* parent, std::shared_ptr<ClientGameModel>  model,
            QPoint position,
            QSize size);

  void paintEvent(QPaintEvent* paint_event) override;

  void RecalculateSizes();
  void MoveButtons();

  void DrawLeveling(QPainter* painter);
  void DrawHealthRect(QPainter* painter);
  void DrawExpRect(QPainter* painter);

  QRectF RectWithPercents(float x1, float y1, float width, float height);

 public slots:
  void Clicked(int index);
  void mousePressEvent(QMouseEvent*) override {};
  void resizeEvent(QResizeEvent*) override;

 private:
  std::shared_ptr<ClientGameModel> model_;
  std::vector<QPushButton*> buttons_;

  float interval_lr_ = 2.f;
  float small_padding_u_ = 2.f;
  float small_interval_lr_ = 0.3f;
  float padding_u_ = 15.f;
  float picture_height_ = 45.f;
  float picture_width_;
  float small_full_width_;
  float small_width_;
  float small_height_;
};

#endif  // GUI_GAMEVIEW_PLAYER_BAR_H_
