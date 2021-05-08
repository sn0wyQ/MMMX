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

const QSize kDefaultSize = QSize(71, 51);
const QPoint kDefaultPos = QPoint(5, 100);
const int kNodes = 5;
const int kBullets = 5;
const int kDefaultWidth = 70;

const QPolygonF kBulletPicture(QVector<QPointF> {
    QPointF(8.f, 0.f),
    QPointF(70.f, 0.f),
    QPointF(70.f, 10.f),
    QPointF(8.f, 10.f),
    QPointF(0.f, 5.f)
});

const QRegion kBulletsRegion(QPolygonF(QVector<QPointF>{
    QPointF(8.f, 0.f),
    QPointF(0.f, 5.f),
    QPointF(8.f, 10.f),
    QPointF(0.f, 15.f),
    QPointF(8.f, 20.f),
    QPointF(0.f, 25.f),
    QPointF(8.f, 30.f),
    QPointF(0.f, 35.f),
    QPointF(8.f, 40.f),
    QPointF(0.f, 45.f),
    QPointF(8.f, 50.f),
    QPointF(70.f, 50.f),
    QPointF(70.f, 0.f)
}).toPolygon());

}  // namespace Constants::ReloadingField

class ReloadingField : public QWidget {
 Q_OBJECT

 public:
  ReloadingField(QWidget* parent, std::shared_ptr<ClientController> controller);

  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent*) override;

 private:
  std::shared_ptr<ClientController> controller_;
  void DrawReloadingField(QPainter* painter);
  void DrawInPercents(QPainter* painter);
};

#endif  // GUI_GAMEVIEW_RELOADING_FIELD_H_
