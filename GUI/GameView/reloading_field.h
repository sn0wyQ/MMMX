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

const float kDefaultBulletWidth = 5.f;
const float kDefaultBulletHeight = 30.f;
const int kDefaultMaxInRows = 10;
const int kDefaultMaxInColumns = 5;
const float kDefaultSpaceBetweenBullets = 1.f;

}  // namespace Constants::ReloadingField

class ReloadingField : public QWidget {
  Q_OBJECT

 public:
  ReloadingField(QWidget* parent,
                 std::shared_ptr<ClientController> controller,
                 QPoint position,
                 QSize size);

  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent*) override;

 private:
  std::shared_ptr<ClientController> controller_;
  void Draw(QPainter* painter);
  void DrawReload(QPainter* painter,
                  int64_t delta_time,
                  int64_t reloading_time);
  void DrawPixmaps(QPainter* painter, int bullets_in_clip);
  void Initialize();
  void RecalculateSize();
  QPixmap bullet_;
  QPixmap empty_bullet_;
  int columns_;
  int rows_;
  bool is_initialize_{false};
};

#endif  // GUI_GAMEVIEW_RELOADING_FIELD_H_
