#ifndef GUI_GAMEVIEW_RELOADINGFIELD_RELOADING_FIELD_H_
#define GUI_GAMEVIEW_RELOADINGFIELD_RELOADING_FIELD_H_

#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

namespace Constants::ReloadingField {

const float kBulletHeight = 120.f;
const float kBulletWidth = 20.f;
const float kMinSpaceBetweenBullets = 1.5f;

}  // namespace Constants::ReloadingField

class ReloadingField : public QWidget {
  Q_OBJECT

 public:
  ReloadingField(QWidget* parent, ClientController* controller);

  void paintEvent(QPaintEvent* event) override;

 private:
  void Draw(QPainter* painter);
  void DrawReload(QPainter* painter,
                  int64_t delta_time,
                  int64_t reloading_time);
  void DrawPixmaps(QPainter* painter, int bullets_in_clip);
  void RecalculateFields();

  ClientController* controller_;
  QPixmap filled_bullet_pixmap_;
  QPixmap unfilled_bullet_pixmap_;
  int bullet_columns_;
  int bullet_rows_;
  int clip_size_;
};

#endif  // GUI_GAMEVIEW_RELOADINGFIELD_RELOADING_FIELD_H_
