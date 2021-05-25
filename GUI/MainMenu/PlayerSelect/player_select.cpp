#include "player_select.h"

PlayerSelect::PlayerSelect(QWidget* parent, Qt::WindowFlags flags)
    : QDialog(parent, flags) {
  label_ = new QLabel("Select player:", this);
  label_->setAlignment(Qt::AlignCenter);
  label_->setObjectName("player_select");

  select_smasher_ = new QPushButton("Shenandoah (Shotgun, Girl)", this);
  connect(select_smasher_,
          &QPushButton::clicked,
          this,
          [this]() { this->done(static_cast<int>(PlayerType::kSmasher)); });

  select_soldier_ = new QPushButton("Jo Cost (Rifle, Easy2Play)", this);
  connect(select_soldier_,
          &QPushButton::clicked,
          this,
          [this]() { this->done(static_cast<int>(PlayerType::kSoldier)); });

  select_spider_ = new QPushButton("Spider 3.0 (Machine Gun, 8 Legs)", this);
  connect(select_spider_,
          &QPushButton::clicked,
          this,
          [this]() { this->done(static_cast<int>(PlayerType::kSpider)); });

  select_viking_ = new QPushButton("Alex Kokhrap (Crossbow, The Best)", this);
  connect(select_viking_,
          &QPushButton::clicked,
          this,
          [this]() { this->done(static_cast<int>(PlayerType::kViking)); });

  cancel_ = new QPushButton("Cancel", this);
  connect(cancel_,
          &QPushButton::clicked,
          this,
          [this]() { this->done(static_cast<int>(PlayerType::kNone)); });
  cancel_->setObjectName("red_big");

  layout_ = new QVBoxLayout(this);
  layout_->addWidget(label_);
  layout_->addWidget(select_smasher_);
  layout_->addWidget(select_soldier_);
  layout_->addWidget(select_spider_);
  layout_->addWidget(select_viking_);
  layout_->addWidget(cancel_);
  this->setLayout(layout_);
}
