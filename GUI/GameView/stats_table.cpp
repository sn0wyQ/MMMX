#include "stats_table.h"

#include <utility>

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
  if (!model_ || !model_->IsLocalPlayerSet()) {
    return;
  }
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  this->DrawTableBackground(&painter);
  this->DrawPlayersStats(&painter);
}

void StatsTable::DrawTableBackground(QPainter* painter) {
  painter->setBrush(QBrush(Qt::black));
  float pen_width = 2;
  QPen pen(Qt::green);
  pen.setWidthF(pen_width);
  painter->setPen(pen);
  int w = this->size().width() - 2 * static_cast<int>(std::ceil(pen_width));
  int h = this->size().height() - 2 * static_cast<int>(std::ceil(pen_width));
  painter->save();
  painter->translate(pen_width, pen_width);
  QSize arc_size = QSize(std::max(w, h) / 20, std::max(w, h) / 20);
  painter->setBrush(QBrush(QColor(128, 128, 255, 128)));
  painter->drawRoundedRect(0, 0, w, h, arc_size.width(), arc_size.height());

  painter->drawLine(0, arc_size.height(),
                    w, arc_size.height());

  painter->restore();
  painter->translate(0, 2 * pen_width + arc_size.height());
}

void StatsTable::DrawPlayersStats(QPainter* painter) {
  painter->save();
  QFont font{};
  painter->setPen(QPen(Qt::black));
  font.setPointSizeF(10.f);
  painter->setFont(font);
  QFontMetrics font_metric(font);
  int text_height = font_metric.height();
  for (const auto& player_stats : model_->GetAllPlayersStats()) {
    painter->drawText(this->size().width() / 100, text_height, player_stats->GetNickname());
    painter->translate(0, text_height);
  }
  painter->restore();
}

void StatsTable::mouseMoveEvent(QMouseEvent* mouse_event) {
  mouse_event->ignore();
}

void StatsTable::mousePressEvent(QMouseEvent* mouse_event) {
  mouse_event->ignore();
}
