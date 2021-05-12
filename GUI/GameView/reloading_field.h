#ifndef GUI_GAMEVIEW_RELOADING_FIELD_H_
#define GUI_GAMEVIEW_RELOADING_FIELD_H_

#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

namespace Constants::ReloadingField {

const float kBulletWidth = 9.f;
const float kBulletHeight = 54.f;
const int kMaxInRows = 10;
const int kMaxInColumns = 7;
const float kSpaceBetweenBullets = 1.5f;
const int kBulletsInRow = 8;

}  // namespace Constants::ReloadingField

class ReloadingField : public QWidget {
  Q_OBJECT

 public:
  ReloadingField(QWidget* parent,
                 std::shared_ptr<ClientController> controller);

  void paintEvent(QPaintEvent* event) override;
  void RecalculateSize();
  QSize RecalculateSize1();

 private:
  std::shared_ptr<ClientController> controller_;
  void Draw(QPainter* painter);
  void DrawReload(QPainter* painter,
                  int64_t delta_time,
                  int64_t reloading_time);
  void DrawPixmaps(QPainter* painter, int bullets_in_clip);
  void RecalculateFields();
  QPixmap bullet_pixmap_;
  QPixmap empty_bullet_pixmap_;
  int in_rows_;
  int in_columns_;
};

#endif  // GUI_GAMEVIEW_RELOADING_FIELD_H_
