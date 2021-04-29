#ifndef GUI_GAMEVIEW_STATS_TABLE_H_
#define GUI_GAMEVIEW_STATS_TABLE_H_

#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

class StatsTable : public QWidget {
  Q_OBJECT

 public:
  StatsTable(QWidget* parent,
             std::shared_ptr<ClientGameModel> model);

  void Show();
  void Hide();

  void paintEvent(QPaintEvent* event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;
  void DrawTable(QPainter* painter);
  void DrawPlayersStats(QPainter* painter);
  std::vector<QRect> header_column_rects_;
  std::vector<QPoint> table_column_points_;
  QPen pen_;
  int column_count_;
  float inner_width_{};
  float inner_height_{};
};

#endif  // GUI_GAMEVIEW_STATS_TABLE_H_