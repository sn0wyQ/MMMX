#include "player_bar.h"

PlayerBar::PlayerBar(QWidget* parent,
                     std::shared_ptr<ClientGameModel>  model)
                     : QWidget(parent), model_(std::move(model)) {}

void PlayerBar::paintEvent(QPaintEvent* paint_event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();
  QPainter painter(this);

  QFont font{};
  font.setPointSizeF(10.f);
  painter.setFont(font);

  painter.setBrush(Qt::white);
  painter.drawRect(0, 0,
                   width(), height());
  painter.drawText(RectWithPercents(0.f, 5.f, 20.f, 20.f),
                   Qt::AlignCenter, "nickname");

  float health_bar_x = 25.f;
  float health_bar_y = 10.f;
  float health_bar_width = 50.f;
  float health_bar_height = 25.f;
  painter.setBrush(Qt::gray);
  painter.drawRect(RectWithPercents(health_bar_x, health_bar_y,
                                    health_bar_width,
                                    health_bar_height));
  painter.setBrush(Qt::darkGreen);
  float hp_ratio = local_player->GetHealthPoints()
      / local_player->GetMaxHealthPoints();
  painter.drawRect(RectWithPercents(
      health_bar_x, health_bar_y,
      health_bar_width * hp_ratio, health_bar_height));

  auto cur_hp = static_cast<int>(local_player->GetHealthPoints());
  auto max_hp = static_cast<int>(local_player->GetMaxHealthPoints());
  float regen_hp_in_sec = local_player->GetHealthRegenSpeed() * 1000.f;
  auto regen_hp_in_sec_int =
      static_cast<int>(std::floor(regen_hp_in_sec * 100.f) / 100.f);
  painter.drawText(RectWithPercents(health_bar_x, health_bar_y,
                                    health_bar_width,
                                    health_bar_height), Qt::AlignCenter,
                   QString::number(cur_hp) + " / "
                   + QString::number(max_hp) + " (+"
                   + QString::number(regen_hp_in_sec_int) + ")");

  float exp_bar_x = 25.f;
  float exp_bar_y = 45.f;
  float exp_bar_width = 50.f;
  float exp_bar_height = 25.f;
  painter.setBrush(Qt::gray);
  painter.drawRect(RectWithPercents(exp_bar_x, exp_bar_y,
                                    exp_bar_width,
                                    exp_bar_height));
  painter.setBrush(Qt::darkYellow);
  float exp_ratio = 0.05f;
  painter.drawRect(RectWithPercents(
      exp_bar_x, exp_bar_y,
      exp_bar_width * exp_ratio, exp_bar_height));
  painter.drawText(RectWithPercents(exp_bar_x, exp_bar_y,
                                    exp_bar_width,
                                    exp_bar_height), Qt::AlignCenter,
                   "Level 0 : (0 / 0)");
}

QRectF PlayerBar::RectWithPercents(
    float x1, float y1, float width, float height) {
  return QRectF(x1 * static_cast<float>(this->width()) / 100.f,
               y1 * static_cast<float>(this->height()) / 100.f,
               width * static_cast<float>(this->width()) / 100.f,
               height * static_cast<float>(this->height()) / 100.f);
}

QPointF PlayerBar::PointWithPercents(float x, float y) {
  return QPointF(x * static_cast<float>(width()) / 100.f,
                 y * static_cast<float>(height()) / 100.f);
}
