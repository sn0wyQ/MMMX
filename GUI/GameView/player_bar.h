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

namespace Constants::PlayerBar {

const float kHealthBarWidth = 30.f;
const float kHealthBarHeight = 25.f;
const float kHealthBarX = 50.f - kHealthBarWidth / 2.f;
const float kHealthBarY = 10.f;

const float kExpBarWidth = 30.f;
const float kExpBarHeight = 25.f;
const float kExpBarX = 50.f - kExpBarWidth / 2.f;
const float kExpBarY = 45.f;

const float kIntervalLr = 2.f;
const float kSmallPaddingU = 2.f;
const float kSmallIntervalLr = 0.3f;
const float kPaddingU = 15.f;
const float kPictureHeight = 45.f;

const std::vector<QString> kLevelingStrings = {
    "MAX HP",
    "REGEN",
    "SPEED",
    "FOV",
    "ACCURACY",
    "BULLET SPEED",
    "RATE OF FIRE",
    "RANGE",
    "DAMAGE",
    "RELOAD"
};

}  // namespace Constants::PlayerBar

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

  float picture_width_;
  float small_full_width_;
  float small_width_;
  float small_height_;
};

#endif  // GUI_GAMEVIEW_PLAYER_BAR_H_
