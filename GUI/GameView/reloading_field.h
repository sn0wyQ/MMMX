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

const QSize kDefaultSize = QSize(70, 50);
const QPoint kDefaultPos = QPoint(5, 10);
const int kNodes = 5;

const QPolygonF reloading_picture(QVector<QPointF> {
    QPointF(8.0, 90.0),
    QPointF(70.0, 90.0),
    QPointF(70.0, 100.0),
    QPointF(8.0, 100.0),
    QPointF(0.0, 95.0)
});

}  // namespace Constants::ReloadingField

class ReloadingField : public QWidget {
 Q_OBJECT

 public:
  ReloadingField(QWidget* parent,
                 std::shared_ptr<ClientGameModel> model);

  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent*) override;

 private:
  std::shared_ptr<ClientGameModel> model_;
  void DrawReloadingField(QPainter* painter);
  void DrawInPercents(QPainter* painter);
  // int64_t clip_size_;
  // int64_t current_clip_size_;
  // int64_t reloading_time_;
};

#endif  // GUI_GAMEVIEW_RELOADING_FIELD_H_
