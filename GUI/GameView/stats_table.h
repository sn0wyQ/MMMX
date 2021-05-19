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
#include "GUI/Animations/spring_emulator.h"

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

const float kSizeAnimationStiffnessRatio = 0.08f;
const float kSizeAnimationFrictionRatio = 0.2f;
const float kOpacityAnimationStiffnessRatio = 0.12f;
const float kOpacityAnimationFrictionRatio = 0.4f;
const float kMinimumSizeRatio = 0.9f;
const float kMaximumSizeRatio = 0.95f;

const std::vector<QString> kColumnNames{
    "Nickname",
    "Level",
    "Kills/Deaths",
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

  void Resize(const QSize& size);
  void paintEvent(QPaintEvent* event) override;

 private:
  QGraphicsOpacityEffect* opacity_effect_;
  std::shared_ptr<ClientGameModel> model_;
  void DrawTable(QPainter* painter);
  void DrawPlayersStats(QPainter* painter);
  std::vector<QRect> header_column_rects_;
  std::vector<QPoint> table_column_points_;
  QPen pen_;
  int column_count_;
  float inner_width_{};
  float inner_height_{};
  float target_size_ratio_;
  float target_opacity_ratio_;
  SpringEmulator<float, false> size_ratio_emulator_;
  SpringEmulator<float, true> opacity_emulator_;
};

#endif  // GUI_GAMEVIEW_STATS_TABLE_H_
