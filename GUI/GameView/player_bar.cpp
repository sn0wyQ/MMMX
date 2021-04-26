#include "player_bar.h"

PlayerBar::PlayerBar(QWidget* parent,
                     std::shared_ptr<ClientGameModel> model,
                     QPoint position,
                     QSize size)
    : QWidget(parent), model_(std::move(model)),
    buttons_(Constants::kLevelingCount) {
  this->move(position);
  this->resize(size);
  picture_width_ = picture_height_ / this->width() * this->height();
  small_full_width_ = picture_width_ / Constants::kCountOfLevels;
  small_width_ = small_full_width_ - small_interval_lr_;
  small_height_ = small_width_ / this->height() * this->width();

  for (int i = 0; i < Constants::kLevelingCount; i++) {
    buttons_[i] = new QPushButton(this);
    auto effect = new QGraphicsOpacityEffect;
    effect->setOpacity(0.f);
    buttons_[i]->setGraphicsEffect(effect);
  }

  MoveButtons();

  for (size_t i = 0; i < buttons_.size(); i++) {
    connect(buttons_[i], &QPushButton::clicked, [this, i] {
      this->Clicked(i);
    });
  }
}

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
  painter.drawRect(0, 0, width(), height());

  DrawLeveling(&painter);
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

void PlayerBar::DrawHealthRect(QPainter* painter) {
  float health_bar_width = 30.f;
  float health_bar_height = 25.f;
  float health_bar_x = 50.f - health_bar_width / 2.f;
  float health_bar_y = 10.f;
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
      static_cast<float>(std::floor(regen_hp_in_sec * 100.f) / 100.f);
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
  float exp_bar_width = 30.f;
  float exp_bar_height = 25.f;
  float exp_bar_x = 50.f - exp_bar_width / 2.f;
  float exp_bar_y = 45.f;
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

void PlayerBar::DrawLeveling(QPainter* painter) {
  auto local_player = model_->GetLocalPlayer();
  auto free_leveling_points = local_player->GetFreeLevelingPoints();
  auto leveling_points = local_player->GetLevelingPoints();

  for (int i = 0; i < Constants::kLevelingCount; i++) {
    int draw_i = i % (Constants::kLevelingCount / 2);
    if (free_leveling_points > 0) {
      QColor clr(Qt::yellow);
      clr.setAlphaF(0.2f);
      painter->setBrush(clr);
    }
    float first = interval_lr_ + (picture_width_ + interval_lr_) * draw_i;
    bool first_part = (i < Constants::kLevelingCount / 2);
    if (!first_part) {
      first = 100 - first - picture_width_;
    }
    painter->drawRect(
        RectWithPercents(first,
                         padding_u_,
                         picture_width_, picture_height_));
    int get_i = first_part ? i : 3 * Constants::kLevelingCount / 2 - 1 - i;
    int current_leveling = leveling_points[get_i];
    painter->setBrush(Qt::darkYellow);
    for (int j = 0; j < Constants::kCountOfLevels; j++) {
      if (j == current_leveling) {
        painter->setBrush(Qt::white);
      }
      painter->drawEllipse(
          RectWithPercents(
              first
                  + small_interval_lr_ + (small_full_width_) * j,
              padding_u_ + picture_height_ + small_padding_u_,
              small_width_, small_height_));
    }
    painter->setBrush(Qt::white);
  }

  std::vector<QString> leveling_strings = {
      "MAX HP",
      "REGEN",
      "SPEED",
      "FOV",
      "BULLET SPEED",
      "RATE OF FIRE",
      "RANGE",
      "DAMAGE"
  };
  for (int i = 0; i < Constants::kLevelingCount; i++) {
    painter->save();
    int draw_i = i % (Constants::kLevelingCount / 2);
    float first = interval_lr_ + (picture_width_ + interval_lr_) * draw_i;
    bool first_part = (i < Constants::kLevelingCount / 2);
    if (!first_part) {
      first = 100 - first - picture_width_;
    }
    auto rect = RectWithPercents(first + picture_width_ / 2.f,
                                 padding_u_ + picture_height_ / 2.f, 0, 0);
    painter->translate(rect.x(), rect.y());
    painter->rotate(45);
    int get_i = first_part ? i : 3 * Constants::kLevelingCount / 2 - 1 - i;
    painter->drawText(
        RectWithPercents(- picture_width_ / 2.f - interval_lr_ / 2.f, - picture_height_ / 2.f,
                         picture_width_ + interval_lr_, picture_height_),
                         Qt::AlignCenter,
        leveling_strings[get_i]);
    painter->restore();
  }
}

void PlayerBar::Clicked(int index) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  auto local_player = model_->GetLocalPlayer();
  int free_leveling_points = local_player->GetFreeLevelingPoints();
  auto leveling_points = local_player->GetLevelingPoints();
  if (free_leveling_points > 0
    && leveling_points[index] < Constants::kCountOfLevels) {
    free_leveling_points--;
    local_player->IncreaseLevelingPoint(index);
    local_player->SetFreeLevelingPoints(free_leveling_points);
  }
}

void PlayerBar::MoveButtons() {
  for (int i = 0; i < Constants::kLevelingCount; i++) {
    int draw_i = i % (Constants::kLevelingCount / 2);
    float first = interval_lr_ + (picture_width_ + interval_lr_) * draw_i;
    bool first_part = (i < Constants::kLevelingCount / 2);
    if (!first_part) {
      first = 100.f - first - picture_width_;
    }
    auto rect = RectWithPercents(first,
                                 padding_u_,
                                 picture_width_, picture_height_);
    int set_i = first_part ? i : 3 * Constants::kLevelingCount / 2 - 1 - i;
    buttons_[set_i]->move(rect.x(), rect.y());
    buttons_[set_i]->resize(rect.width(), rect.height());
  }
}
