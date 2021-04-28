#include "stats_table.h"

#include <algorithm>
#include <utility>

namespace Constants::StatsTable {

const QColor kTableColor = Qt::black;
const QColor kBackgroundColor = QColor(128, 128, 128, 128);
const QColor kHeaderTextColor = Qt::green;
const QColor kTextColor = Qt::black;
const QColor kLocalPLayerTextColor = Qt::yellow;
const int kExternalLinesWidth = 3;
const int kInternalLinesWidth = 1;

const std::vector<QString> kColumnNames{
    "Nickname",
    "Level",
    "Kills",
    "Deaths"
};

}  // namespace Constants::StatsTable

StatsTable::StatsTable(QWidget* parent,
                       std::shared_ptr<ClientGameModel> model,
                       QPoint position,
                       QSize size) :
    QWidget(parent),
    model_{std::move(model)} {
  this->move(position);
  this->resize(size);
  this->setMouseTracking(true);
}

void StatsTable::paintEvent(QPaintEvent* paint_event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  this->DrawTable(&painter);
  this->DrawPlayersStats(&painter);
}

void StatsTable::DrawTable(QPainter* painter) {
  column_count_ = static_cast<int>(Constants::StatsTable::kColumnNames.size());
  pen_.setColor(Constants::StatsTable::kTableColor);
  pen_.setWidth(Constants::StatsTable::kExternalLinesWidth);
  painter->setPen(pen_);
  int w = this->size().width() - 2 * Constants::StatsTable::kExternalLinesWidth;
  int h =
      this->size().height() - 2 * Constants::StatsTable::kExternalLinesWidth;
  painter->save();
  painter->translate(Constants::StatsTable::kExternalLinesWidth,
                     Constants::StatsTable::kExternalLinesWidth);
  QSize arc_size = QSize(std::max(w, h) / 20, std::max(w, h) / 20);
  painter->setBrush(QBrush(Constants::StatsTable::kBackgroundColor));
  painter->drawRoundedRect(0, 0, w, h, arc_size.width(), arc_size.height());
  painter->drawLine(0, arc_size.height(), w, arc_size.height());

  header_column_rects_.clear();
  table_column_points_.clear();

  for (int i = 0; i < column_count_; i++) {
    if (i + 1 < column_count_) {
      painter->drawLine((i + 1) * w / column_count_, 0,
                        (i + 1) * w / column_count_, arc_size.height());
    }
    header_column_rects_.emplace_back(i * w / column_count_, 0,
                                      w / column_count_,
                                      arc_size.height());
  }
  pen_.setWidth(Constants::StatsTable::kInternalLinesWidth);
  painter->setPen(pen_);
  for (int i = 0; i < column_count_; i++) {
    if (i + 1 < column_count_) {
      painter->drawLine((i + 1) * w / column_count_, arc_size.height(),
                        (i + 1) * w / column_count_, h);
    }
    table_column_points_.emplace_back(i * w / column_count_,
                                      arc_size.height());
  }

  painter->restore();
}

void StatsTable::DrawPlayersStats(QPainter* painter) {
  painter->save();
  QFont font{};
  font.setPixelSize(this->size().height() / 25);
  painter->setFont(font);
  int text_height = 2 * QFontMetrics(font).height();
  auto stats = model_->GetAllPlayersStats();
  std::sort(stats.begin(),
            stats.end(),
            [](const std::shared_ptr<PlayerStats>& stats1,
               const std::shared_ptr<PlayerStats>& stats2) {
              return *stats1 < *stats2;
            });
  pen_.setColor(Constants::StatsTable::kHeaderTextColor);
  painter->setPen(pen_);
  for (int i = 0; i < column_count_; i++) {
    painter->drawText(header_column_rects_[i],
                      Qt::AlignCenter,
                      Constants::StatsTable::kColumnNames[i]);
  }
  auto offset_x = QPoint(this->size().width() / column_count_, 0);
  auto prev_offset_y = QPoint();
  auto curr_offset_y = QPoint(0, text_height);
  auto get_rect = [&](int i) {
    return QRect(table_column_points_[i] + prev_offset_y,
                 table_column_points_[i] + offset_x + curr_offset_y);
  };
  for (const auto& stat : stats) {
    if (stat->GetPlayerId() == model_->GetLocalPlayer()->GetId()) {
      pen_.setColor(Constants::StatsTable::kLocalPLayerTextColor);
    } else {
      pen_.setColor(Constants::StatsTable::kTextColor);
    }
    painter->setPen(pen_);
    painter->drawText(get_rect(0), Qt::AlignCenter, stat->GetNickname());
    painter->drawText(get_rect(1), Qt::AlignCenter,
                      QString::number(stat->GetLevel()));
    painter->drawText(get_rect(2), Qt::AlignCenter,
                      QString::number(stat->GetKills()));
    painter->drawText(get_rect(3), Qt::AlignCenter,
                      QString::number(stat->GetDeaths()));
    pen_.setColor(Constants::StatsTable::kTableColor);
    painter->setPen(pen_);

    painter->drawLine(get_rect(0).bottomLeft(),
                      get_rect(3).bottomRight());
    prev_offset_y = curr_offset_y;
    curr_offset_y.ry() += text_height;
  }
  painter->restore();
}
