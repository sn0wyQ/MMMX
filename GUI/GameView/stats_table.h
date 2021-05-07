#ifndef GUI_GAMEVIEW_STATS_TABLE_H_
#define GUI_GAMEVIEW_STATS_TABLE_H_

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QWidget>

#include "Client/client_controller.h"
#include "Painter/painter.h"

namespace Constants::StatsTable {

const QFont kHeaderFont("Roboto Mono", -1, QFont::Weight::Bold);
const QFont kTextFont("Roboto Mono");

const QColor kBackgroundColor(86, 86, 86, 180);
const QColor kHeaderTextColor(95, 255, 62, 180);
const QColor kTextColor(235, 235, 235, 180);
const QColor kLocalPLayerTextColor(255, 235, 235, 255);
const QColor kFrameColor(62, 62, 62, 255);
const float kPenWidthMultiplier = 0.005f;
const int kMaxPlayerInTable = 10;

const std::vector<QString> kColumnNames{
    "Nickname",
    "Level",
    "Kills",
    "Deaths"
};

}  // namespace Constants::StatsTable

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
