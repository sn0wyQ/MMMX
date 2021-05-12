#ifndef GUI_GAMEVIEW_TIME_BAR_H_
#define GUI_GAMEVIEW_TIME_BAR_H_

#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class TimeBar : public QWidget {
  Q_OBJECT

 public:
  TimeBar(QWidget* parent, std::shared_ptr<ClientController> controller,
            QPoint position, QSize size);

  void paintEvent(QPaintEvent* paint_event) override;

  static QString FormattedTime(int64_t msecs);
  static QString AddNulls(int64_t x, int cnt);

 public slots:
  void mousePressEvent(QMouseEvent*) override {};

 private:
  std::shared_ptr<ClientController> controller_;
};

#endif  // GUI_GAMEVIEW_TIME_BAR_H_
