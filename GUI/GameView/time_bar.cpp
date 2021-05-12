#include "time_bar.h"

TimeBar::TimeBar(QWidget* parent, std::shared_ptr<ClientController> controller,
                 QPoint position, QSize size) : QWidget(parent),
                 controller_(std::move(controller)) {
  this->resize(size);
  this->move(position);
}

void TimeBar::paintEvent(QPaintEvent* paint_event) {
  if (!controller_->GetModel()->IsLocalPlayerSet()) {
    return;
  }
  if (controller_->IsTickingBlocked()) {
    return;
  }

  QPainter painter(this);

  QFont font(Constants::Painter::kDefaultFont);
  font.setPointSizeF(25.f);
  painter.setFont(font);

  painter.setBrush(Qt::white);
  QString text;
  if (controller_->GetModel()->IsGameStarted()) {
    text = FormattedTime(controller_->GetGameRemainingTime());
  } else {
    text = "WARMUP\n" + FormattedTime(controller_->GetWarmupRemainingTime());
  }
  painter.drawText(QRectF(0, 0, width(), height()),
                   Qt::AlignCenter,
                   text);
}

QString TimeBar::FormattedTime(int64_t msecs) {
  int64_t full_secs = msecs / 1000;
  int64_t secs = full_secs % 60;
  int64_t minutes = full_secs / 60;
  return AddNulls(minutes, 2) + " : " + AddNulls(secs, 2);
}

QString TimeBar::AddNulls(int64_t x, int cnt) {
  QString result = QString::number(x);
  std::reverse(result.begin(), result.end());
  while (result.size() < cnt) {
    result.push_back('0');
  }
  std::reverse(result.begin(), result.end());
  return result;
}
