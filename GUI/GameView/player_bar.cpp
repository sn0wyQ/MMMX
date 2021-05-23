#include "player_bar.h"

using Constants::PlayerBar::kHealthBarHeight;
using Constants::PlayerBar::kHealthBarWidth;
using Constants::PlayerBar::kHealthBarX;
using Constants::PlayerBar::kHealthBarY;

using Constants::PlayerBar::kExpBarHeight;
using Constants::PlayerBar::kExpBarWidth;
using Constants::PlayerBar::kExpBarX;
using Constants::PlayerBar::kExpBarY;

using Constants::PlayerBar::kIntervalLr;
using Constants::PlayerBar::kSmallIntervalLr;
using Constants::PlayerBar::kPaddingU;
using Constants::PlayerBar::kSmallPaddingU;
using Constants::PlayerBar::kPictureHeight;
using Constants::PlayerBar::kLevelingNames;
using Constants::PlayerBar::kUpdatePixmapHeight;
using Constants::PlayerBar::kBasePath;

PlayerBar::PlayerBar(QWidget* parent, std::shared_ptr<ClientGameModel> model,
                     QPoint position, QSize size)
    : QWidget(parent), hp_emulator_(0.5f, 0.8f), xp_emulator_(0.5f, 0.8f),
      model_(std::move(model)),
      buttons_(static_cast<int>(LevelingSlots::SIZE)),
      leveling_pixmaps_(static_cast<int>(LevelingSlots::SIZE)){
  hp_emulator_.SetCurrentValue(0.f);
  xp_emulator_.SetCurrentValue(0.f);

  this->move(position);
  this->resize(size);
  this->RecalculateSizes();

  for (int i = 0; i < static_cast<int>(LevelingSlots::SIZE); i++) {
    buttons_[i] = new QPushButton(this);
    auto effect = new QGraphicsOpacityEffect;
    effect->setOpacity(0.f);
    buttons_[i]->setGraphicsEffect(effect);
    buttons_[i]->setFocusPolicy(Qt::NoFocus);
    buttons_[i]->setToolTip(kLevelingNames[i]);
  }

  MoveButtons();

  for (size_t i = 0; i < buttons_.size(); i++) {
    connect(buttons_[i], &QPushButton::clicked, [this, i] {
      this->Clicked(i);
    });
  }

  this->InitializePixmapIcons();
}

void PlayerBar::paintEvent(QPaintEvent* paint_event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }

  auto local_player = model_->GetLocalPlayer();
  QPainter painter(this);
  Constants::SetPainterHints(&painter);

  QFont font(Constants::Painter::kDefaultFont);
  font.setPointSizeF(10.f);
  painter.setFont(font);

  painter.setBrush(Qt::white);
  painter.drawRect(0, 0, width(), height());

  DrawLevelingButtons(&painter);
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
  painter->setBrush(Qt::gray);
  painter->drawRoundedRect(RectWithPercents(kHealthBarX, kHealthBarY,
                                            kHealthBarWidth,
                                            kHealthBarHeight), 20, 20);
  painter->setBrush(Qt::darkGreen);
  auto local_player = model_->GetLocalPlayer();
  auto cur_hp = static_cast<int>(local_player->GetHealthPoints());
  auto max_hp = static_cast<int>(local_player->GetMaxHealthPoints());
  hp_emulator_.SetBounds(0, max_hp);
  hp_emulator_.MakeStepTo(cur_hp);
  float hp_ratio = hp_emulator_.GetCurrentValue() / max_hp;
  painter->setClipRect(RectWithPercents(
      kHealthBarX, kHealthBarY,
      kHealthBarWidth * hp_ratio, kHealthBarHeight));
  painter->drawRoundedRect(RectWithPercents(kHealthBarX, kHealthBarY,
                                            kHealthBarWidth,
                                            kHealthBarHeight), 20, 20);
  painter->setClipping(false);
  float regen_hp_in_sec = local_player->GetHealthRegenRate() * 1000.f;
  auto regen_hp_in_sec_int =
      static_cast<float>(std::floor(regen_hp_in_sec * 100.f) / 100.f);
  painter->drawText(RectWithPercents(kHealthBarX, kHealthBarY,
                                     kHealthBarWidth,
                                     kHealthBarHeight), Qt::AlignCenter,
                    QString::number(cur_hp) + " / "
                        + QString::number(max_hp) + " (+"
                        + QString::number(regen_hp_in_sec_int) + ")");
}

void PlayerBar::DrawExpRect(QPainter* painter) {
  auto local_player = model_->GetLocalPlayer();
  int cur_level = local_player->GetLevel();
  auto cur_exp = static_cast<int>(local_player->GetCurrentExp());
  float exp_for_level = Constants::GetExpForLevel(cur_level);

  painter->setBrush(Qt::gray);
  painter->drawRoundedRect(RectWithPercents(kExpBarX, kExpBarY,
                                            kExpBarWidth,
                                            kExpBarHeight), 20, 20);
  painter->setBrush(Qt::cyan);
  xp_emulator_.SetBounds(0.f, exp_for_level);
  xp_emulator_.MakeStepTo(cur_exp);
  float exp_ratio = xp_emulator_.GetCurrentValue() / exp_for_level;
  painter->setClipRect(RectWithPercents(
      kExpBarX, kExpBarY,
      kExpBarWidth * exp_ratio, kExpBarHeight));
  painter->drawRoundedRect(RectWithPercents(kExpBarX, kExpBarY,
                                            kExpBarWidth,
                                            kExpBarHeight), 20, 20);
  painter->setClipping(false);
  painter->drawText(RectWithPercents(kExpBarX, kExpBarY,
                                     kExpBarWidth,
                                     kExpBarHeight), Qt::AlignCenter,
                    "Level " + QString::number(cur_level) + " : ("
                        + QString::number(cur_exp) + " / "
                        + QString::number(exp_for_level) + ")");
}

void PlayerBar::DrawLevelingButtons(QPainter* painter) {
  auto local_player = model_->GetLocalPlayer();
  auto free_leveling_points = local_player->GetFreeLevelingPoints();
  auto leveling_points = local_player->GetLevelingPoints();
  for (int i = 0; i < static_cast<int>(LevelingSlots::SIZE); i++) {
    int draw_i = i % (static_cast<int>(LevelingSlots::SIZE) / 2);

    bool first_part = (i < static_cast<int>(LevelingSlots::SIZE) / 2);
    float position_x = first_part ?
        (kIntervalLr + (picture_width_ + kIntervalLr) * draw_i) :
        (100 - (picture_width_ + kIntervalLr) * (draw_i + 1));
    int get_i = first_part ? i : 3
        * static_cast<int>(LevelingSlots::SIZE) / 2 - 1 - i;

    auto rect = RectWithPercents(position_x,
                                 kPaddingU,
                                 picture_width_,
                                 kPictureHeight);
    painter->drawRect(rect);
    painter->drawPixmap(rect.x(), rect.y(), leveling_pixmaps_[get_i]);

    if (leveling_points[get_i] == Constants::kCountOfLevels) {
      painter->drawPixmap(rect.x(),
                          rect.y() - max_lvl_button_pixmap_.height(),
                          max_lvl_button_pixmap_);
    } else {
      if (free_leveling_points > 0) {
        painter->drawPixmap(rect.x(),
                            rect.y() - update_lvl_button_pixmap_.height(),
                            update_lvl_button_pixmap_);
      }
    }

    // draw level of a current button
    int current_leveling = leveling_points[get_i];
    painter->setBrush(Qt::darkYellow);
    for (int j = 0; j < Constants::kCountOfLevels; j++) {
      if (j == current_leveling) {
        painter->setBrush(Qt::white);
      }
      painter->drawEllipse(
          RectWithPercents(
              position_x + (small_full_width_) * j,
              kPaddingU + kPictureHeight + kSmallPaddingU,
              small_width_, small_height_));
    }
    painter->setBrush(Qt::white);
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
    local_player->IncreaseLevelingPoint(
        static_cast<LevelingSlots>(index));
    local_player->SetFreeLevelingPoints(free_leveling_points);
  }
}

void PlayerBar::MoveButtons() {
  this->RecalculateSizes();
  for (int i = 0; i < static_cast<int>(LevelingSlots::SIZE); i++) {
    int draw_i = i % (static_cast<int>(LevelingSlots::SIZE) / 2);
    bool first_part = (i < static_cast<int>(LevelingSlots::SIZE) / 2);
    float position_x = first_part ?
                       (kIntervalLr + (picture_width_ + kIntervalLr) * draw_i) :
                       (100 - (picture_width_ + kIntervalLr) * (draw_i + 1));
    auto rect = RectWithPercents(position_x,
                                 kPaddingU,
                                 picture_width_, kPictureHeight);
    int set_i = first_part ? i : 3
        * static_cast<int>(LevelingSlots::SIZE) / 2 - 1 - i;
    buttons_[set_i]->move(rect.x(), rect.y());
    buttons_[set_i]->resize(rect.width(), rect.height());
  }
}

void PlayerBar::RecalculateSizes() {
  picture_width_ = kPictureHeight / this->width() * this->height();
  small_width_ =
      (picture_width_ - (Constants::kCountOfLevels - 1) * kSmallIntervalLr)
          / Constants::kCountOfLevels;
  small_full_width_ = small_width_ + kSmallIntervalLr;
  small_height_ = small_width_ / this->height() * this->width();
}

void PlayerBar::resizeEvent(QResizeEvent*) {
  MoveButtons();
}

void PlayerBar::InitializePixmapIcons() {
  float current_width = this->height() * 0.45f;
  for (size_t i = 0; i < leveling_pixmaps_.size(); i++) {
    QString path = kBasePath + kLevelingNames[i] + ".svg";
    QSvgRenderer current_icon_render(path);
    leveling_pixmaps_[i] = QPixmap(current_width, current_width);
    leveling_pixmaps_[i].fill(Qt::transparent);
    QPainter painter_for_current_icon(&leveling_pixmaps_[i]);
    current_icon_render.render(&painter_for_current_icon,
                               leveling_pixmaps_[i].rect());
  }
  QSvgRenderer update_button_render(kBasePath + "UPDATE LVL.svg");
  update_lvl_button_pixmap_ = QPixmap(current_width, this->height() * 0.2f);
  update_lvl_button_pixmap_.fill(Qt::transparent);
  QPainter painter_for_update(&update_lvl_button_pixmap_);
  update_button_render.render(&painter_for_update,
                             update_lvl_button_pixmap_.rect());
  QSvgRenderer max_button_render(kBasePath + "MAX LVL.svg");
  max_lvl_button_pixmap_ = QPixmap(current_width, this->height() * 0.2f);
  max_lvl_button_pixmap_.fill(Qt::transparent);
  QPainter painter_for_max(&max_lvl_button_pixmap_);
  max_button_render.render(&painter_for_max,
                              max_lvl_button_pixmap_.rect());
}
