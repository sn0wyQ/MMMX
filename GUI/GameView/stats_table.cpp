#include "stats_table.h"

#include <algorithm>
#include <utility>

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

StatsTable::StatsTable(QWidget* parent,
                       std::shared_ptr<ClientGameModel> model) :
    QWidget(parent),
    model_{std::move(model)} {
  column_count_ = static_cast<int>(Constants::StatsTable::kColumnNames.size());
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
  painter->save();
  painter->setPen(pen_);
  float w = this->width();
  float h = this->height();
  QSize arc_size = QSize(std::max(w, h) / 20.f, std::max(w, h) / 20.f);
  painter->setPen(Qt::NoPen);
  painter->setBrush(Constants::StatsTable::kBackgroundColor);
  painter->drawRoundedRect(0, 0, w, h,
                           arc_size.width(), arc_size.height());

  header_column_rects_.clear();
  table_column_points_.clear();
  inner_width_ = w * 0.8f;
  float offset_x = (w - inner_width_) / 2.f;
  inner_height_ = h * 0.85f;
  float offset_y = (h - inner_height_) / 2.f;
  float frame_height = offset_y / 2.f;
  float frame_width = w * 0.9f;
  float frame_offset_x = (w - frame_width) / 2.f;

  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(Constants::StatsTable::kFrameColor,
                       w * Constants::StatsTable::kPenWidthMultiplier));
  painter->drawRoundedRect(frame_offset_x, offset_y - frame_height,
                           frame_width,
                           arc_size.height() + frame_height,
                           arc_size.width() / 2.f, arc_size.height() / 2.f);

  for (int i = 0; i < column_count_; i++) {
    header_column_rects_.emplace_back(
        offset_x + i * inner_width_ / column_count_,
        offset_y - frame_height,
        inner_width_ / column_count_,
        arc_size.height() + frame_height);
  }
  for (int i = 0; i < column_count_; i++) {
    table_column_points_.emplace_back(
        offset_x + i * inner_width_ / column_count_,
        arc_size.height() + 1.5f * offset_y);
  }

  painter->restore();
}

void StatsTable::DrawPlayersStats(QPainter* painter) {
  painter->save();
  QFont font(Constants::StatsTable::kHeaderFont);
  font.setPixelSize(this->height() / 23);
  painter->setFont(font);
  auto stats = model_->GetAllPlayersStats();
  std::sort(stats.begin(),
            stats.end(),
            [](const std::shared_ptr<PlayerStats>& stats1,
               const std::shared_ptr<PlayerStats>& stats2) {
              return *stats1 < *stats2;
            });
  if (stats.size() > Constants::StatsTable::kMaxPlayerInTable) {
    stats.resize(Constants::StatsTable::kMaxPlayerInTable);
  }
  pen_.setColor(Constants::StatsTable::kHeaderTextColor);
  painter->setPen(pen_);
  for (int i = 0; i < column_count_; i++) {
    painter->drawText(header_column_rects_[i],
                      Qt::AlignCenter,
                      Constants::StatsTable::kColumnNames[i]);
  }
  auto offset_x = QPoint(inner_width_ / column_count_, 0);
  auto prev_offset_y = QPoint();
  font = Constants::StatsTable::kTextFont;
  font.setPixelSize(this->height() / 25);
  painter->setFont(font);
  int text_height = 1.7f * QFontMetrics(font).height();
  auto curr_offset_y = QPoint(0, text_height);
  auto get_rect = [this, &prev_offset_y, &offset_x, &curr_offset_y](int i) {
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
    painter->drawText(get_rect(0),
                      Qt::AlignCenter,
                      stat->GetNickname());
    painter->drawText(get_rect(1), Qt::AlignCenter,
                      QString::number(stat->GetLevel()));
    painter->drawText(get_rect(2), Qt::AlignCenter,
                      QString::number(stat->GetKills()));
    painter->drawText(get_rect(3),
                      Qt::AlignCenter,
                      QString::number(stat->GetDeaths()));
    painter->setPen(pen_);

    prev_offset_y = curr_offset_y;
    curr_offset_y.ry() += text_height;
  }
  painter->restore();
}

void StatsTable::Hide() {
  auto effect = new QGraphicsOpacityEffect;
  effect->setOpacity(0.01f);
  this->setGraphicsEffect(effect);
}

void StatsTable::Show() {
  auto effect = new QGraphicsOpacityEffect;
  effect->setOpacity(1.f);
  this->setGraphicsEffect(effect);
}
