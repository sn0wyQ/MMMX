#ifndef GUI_GAMEVIEW_PLAYER_BAR_H_
#define GUI_GAMEVIEW_PLAYER_BAR_H_

#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QPushButton>
#include <QWidget>

#include "Client/client_controller.h"
#include "GUI/Animations/spring_emulator.h"
#include "Painter/painter.h"

namespace Constants::PlayerBar {
  const float kHealthBarHeight = 25.f;
  const float kHealthBarWidth = 30.f;
  const float kHealthBarX = 50.f - kHealthBarWidth / 2.f;
  const float kHealthBarY = 10.f;

  const float kExpBarHeight = 25.f;
  const float kExpBarWidth = 30.f;
  const float kExpBarX = 50.f - kExpBarWidth / 2.f;
  const float kExpBarY = 45.f;

  const float kIntervalLr = 2.f;
  const float kSmallIntervalLr = 0.3f;
  const float kPaddingU = 25.f;
  const float kSmallPaddingU = 2.f;
  const float kPictureHeight = 45.f;

  const float kUpdatePixmapHeight = 10.f;
  const QString kBasePath = "./Res/Icons/";

  const std::vector<QString> kLevelingNames = {
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
}

class PlayerBar : public QWidget {
  Q_OBJECT

 public:
  PlayerBar(QWidget* parent, std::shared_ptr<ClientGameModel>  model,
            QPoint position, QSize size);

  void paintEvent(QPaintEvent* paint_event) override;

  void RecalculateSizes();
  void MoveButtons();

  void DrawLevelingButtons(QPainter* painter);
  void DrawHealthRect(QPainter* painter);
  void DrawExpRect(QPainter* painter);
  void DrawUpdateAndMaxButtons(QPainter* painter);

  QRectF RectWithPercents(float x1, float y1, float width, float height);

  void InitializePixmapIcons();

 public slots:
  void Clicked(int index);
  void mousePressEvent(QMouseEvent*) override {};
  void resizeEvent(QResizeEvent*) override;

 private:
  SpringEmulator<float, true> hp_emulator_;
  SpringEmulator<float, true> xp_emulator_;

  std::shared_ptr<ClientGameModel> model_;
  std::vector<QPushButton*> buttons_;

  std::vector<QPixmap> leveling_pixmaps_;
  QPixmap update_lvl_button_pixmap_;
  QPixmap max_lvl_button_pixmap_;

  float picture_width_;
  float small_full_width_;
  float small_width_;
  float small_height_;
};

#endif  // GUI_GAMEVIEW_PLAYER_BAR_H_
