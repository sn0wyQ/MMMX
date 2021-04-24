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

  DrawNickname(&painter);
  DrawHealthRect(&painter);
  DrawExpRect(&painter);
}

QRectF PlayerBar::RectWithPercents(
    float x1, float y1, float width, float height) {
  return QRectF(x1 * static_cast<float>(this->width()) / 100.f,
               y1 * static_cast<float>(this->height()) / 100.f,
               width * static_cast<float>(this->width()) / 100.f,
               height * static_cast<float>(this->height()) / 100.f);
}

void PlayerBar::DrawNickname(QPainter* painter) {
  painter->setBrush(Qt::white);
  painter->drawRect(0, 0,
                   width(), height());
  painter->drawText(RectWithPercents(0.f, 5.f, 20.f, 20.f),
                   Qt::AlignCenter, "nickname");
}

void PlayerBar::DrawHealthRect(QPainter* painter) {
  float health_bar_x = 25.f;
  float health_bar_y = 10.f;
  float health_bar_width = 50.f;
  float health_bar_height = 25.f;
  painter->setBrush(Qt::gray);
  painter->drawRect(RectWithPercents(health_bar_x, health_bar_y,
                                    health_bar_width,
                                    health_bar_height));
  painter->setBrush(Qt::darkGreen);
  auto local_player = model_->GetLocalPlayer();
  float hp_ratio = local_player->GetHealthPoints()
      / local_player->GetMaxHealthPoints();
  painter->drawRect(RectWithPercents(
      health_bar_x, health_bar_y,
      health_bar_width * hp_ratio, health_bar_height));
  auto cur_hp = static_cast<int>(local_player->GetHealthPoints());
  auto max_hp = static_cast<int>(local_player->GetMaxHealthPoints());
  float regen_hp_in_sec = local_player->GetHealthRegenRate() * 1000.f;
  auto regen_hp_in_sec_int =
      static_cast<int>(std::floor(regen_hp_in_sec * 100.f) / 100.f);
  painter->drawText(RectWithPercents(health_bar_x, health_bar_y,
                                    health_bar_width,
                                    health_bar_height), Qt::AlignCenter,
                   QString::number(cur_hp) + " / "
                       + QString::number(max_hp) + " (+"
                       + QString::number(regen_hp_in_sec_int) + ")");
}

void PlayerBar::DrawExpRect(QPainter* painter) {
  auto local_player = model_->GetLocalPlayer();
  int cur_level = local_player->GetLevel();
  auto cur_exp = static_cast<int>(local_player->GetCurrentExp());
  float exp_for_level = Constants::kExpForLevel[cur_level - 1];
  float exp_bar_x = 25.f;
  float exp_bar_y = 45.f;
  float exp_bar_width = 50.f;
  float exp_bar_height = 25.f;
  painter->setBrush(Qt::gray);
  painter->drawRect(RectWithPercents(exp_bar_x, exp_bar_y,
                                    exp_bar_width,
                                    exp_bar_height));
  painter->setBrush(Qt::darkYellow);
  float exp_ratio = local_player->GetCurrentExp()
      / exp_for_level;
  painter->drawRect(RectWithPercents(
      exp_bar_x, exp_bar_y,
      exp_bar_width * exp_ratio, exp_bar_height));
  painter->drawText(RectWithPercents(exp_bar_x, exp_bar_y,
                                    exp_bar_width,
                                    exp_bar_height), Qt::AlignCenter,
                   "Level " + QString::number(cur_level) + " : ("
                       + QString::number(cur_exp) + " / "
                       + QString::number(exp_for_level) + ")");
}
