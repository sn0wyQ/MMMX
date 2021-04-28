#include "stats_table.h"

#include <algorithm>
#include <utility>

namespace Constants::StatsTable {

const QColor kBackgroundColor = QColor(86, 86, 86, 180);
QFont kHeaderFont("Roboto Mono", 31);
const QColor kHeaderTextColor = QColor(95, 255, 62, 180);
const QFont kTextFont("Roboto Mono", 25);
const QColor kTextColor = QColor(236, 236, 236, 180);
const QColor kLocalPLayerTextColor = QColor(62, 62, 62, 255);
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
  Constants::StatsTable::kHeaderFont.setWeight(QFont::Weight::Bold);
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
  painter->setPen(pen_);
  int w = this->size().width();
  int h = this->size().height();
  painter->save();
  QSize arc_size = QSize(std::max(w, h) / 20, std::max(w, h) / 20);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(Constants::StatsTable::kBackgroundColor));
  painter->drawRoundedRect(0, 0, w, h, arc_size.width(), arc_size.height());

  header_column_rects_.clear();
  table_column_points_.clear();

  for (int i = 0; i < column_count_; i++) {
    header_column_rects_.emplace_back(i * w / column_count_, 0,
                                      w / column_count_,
                                      arc_size.height());
  }
  painter->setPen(pen_);
  for (int i = 0; i < column_count_; i++) {
    table_column_points_.emplace_back(i * w / column_count_,
                                      arc_size.height());
  }

  painter->restore();
}

void StatsTable::DrawPlayersStats(QPainter* painter) {
  painter->save();
  QFont font(Constants::StatsTable::kHeaderFont);
  font.setPixelSize(this->size().height() / 20);
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
  font = Constants::StatsTable::kTextFont;
  font.setPixelSize(this->size().height() / 20);
  painter->setFont(font);
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
    painter->setPen(pen_);

    prev_offset_y = curr_offset_y;
    curr_offset_y.ry() += text_height;
  }
  painter->restore();
}

void StatsTable::Hide() {
  auto effect = new QGraphicsOpacityEffect;
  effect->setOpacity(0.f);
  this->setGraphicsEffect(effect);
}

void StatsTable::Show() {
  auto effect = new QGraphicsOpacityEffect;
  effect->setOpacity(1.f);
  this->setGraphicsEffect(effect);
}
