#ifndef GUI_GAMEVIEW_STATS_TABLE_H_
#define GUI_GAMEVIEW_STATS_TABLE_H_

#include <memory>
#include <utility>

#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class StatsTable : public QWidget {
 Q_OBJECT

 public:
  StatsTable(QWidget* parent,
             std::shared_ptr<ClientGameModel> model,
             QPoint position,
             QSize size);

  void paintEvent(QPaintEvent* event) override;
  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void mousePressEvent(QMouseEvent* mouse_event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;
  void DrawTable(QPainter* painter);
  void DrawPlayersStats(QPainter* painter);
  std::vector<QPoint> header_column_points_;
  std::vector<QPoint> table_column_point_;
  size_t column_count_;
};

#endif  // GUI_GAMEVIEW_STATS_TABLE_H_
