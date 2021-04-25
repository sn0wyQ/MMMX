#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {}

Player::Player(const Player& other) : Entity(other) {
  is_local_player_ = other.is_local_player_;
  weapon_type_ = other.weapon_type_;
  switch (weapon_type_) {
    case WeaponType::kMachineGun: {
      weapon_ = std::make_shared<MachineGun>(
          *(std::dynamic_pointer_cast<MachineGun>(other.weapon_)));
      break;
    }
  }
  current_exp_ = other.current_exp_;
  level_ = other.level_;
}

void Player::SetParams(std::vector<QVariant> params) {
  SetCurrentExp(params.back().toFloat());
  params.pop_back();
  SetLevel(params.back().toInt());
  params.pop_back();
  auto weapon_type = static_cast<WeaponType>(params.back().toInt());
  weapon_type_ = weapon_type;
  switch (weapon_type) {
    case WeaponType::kMachineGun: {
      weapon_ = std::make_shared<MachineGun>();
      break;
    }
  }
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result = Entity::GetParams();
  result.emplace_back(static_cast<int>(weapon_type_));
  result.emplace_back(GetLevel());
  result.emplace_back(GetCurrentExp());
  return result;
}

void Player::DrawLevel(Painter* painter) {
  QPointF translation(0.f, -GetHeight() * 1.7f);
  painter->Translate(translation);
  float rect_width = 75.f;
  float rect_height = 14.f;
  QFont font{};
  font.setPointSizeF(7.f);
  painter->setFont(font);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);
  auto cur_hp = static_cast<int>(this->GetHealthPoints());
  auto max_hp = static_cast<int>(this->GetMaxHealthPoints());
  painter->drawText(text_rect, Qt::AlignCenter,
                    QString::number(this->GetLevel()));
  // auto pen = painter->pen();
  // QColor clr_back = Qt::gray;
  // clr_back.setAlphaF(0.4f);
  // painter->setPen(clr_back);
  // painter->drawRect(-rect_width / 2.f, -rect_height / 2.f,
  //                   rect_width, rect_height);
  // auto brush = painter->brush();
  // QColor clr_main = Qt::darkGreen;
  // clr_main.setAlphaF(0.4f);
  // painter->setBrush(clr_main);
  // float part = this->GetHealthPoints() / this->GetMaxHealthPoints();
  // float width = part * rect_width;
  // painter->drawRect(-rect_width / 2.f, -rect_height / 2.f,
  //                   width, rect_height);
  // painter->setBrush(brush);
  // painter->setPen(pen);
  painter->Translate(-translation);
}

void Player::DrawRelatively(Painter* painter) {
  // Body [Temporary]
  painter->DrawEllipse(QPointF(), GetRadius(), GetRadius());

  // Eyes [Temporary]
  painter->DrawEllipse(QPointF(0.4f * GetRadius(),
                               -0.3f * GetRadius()),
                       0.2f,
                       0.2f);
  painter->DrawEllipse(
      QPointF(0.4f * GetRadius(), 0.3f * GetRadius()),
      0.2f, 0.2f);

  // Direction Arrow [Needs improvement to look cuter]
  if (IsLocalPlayer()) {
    painter->DrawTriangle(QPointF(GetRadius() + 0.2f, -0.3f),
                          QPointF(GetRadius() + 0.2f, 0.3f),
                          QPointF(GetRadius() + 0.7f, 0.f));
  }
  weapon_->DrawWeapon(painter);
}

bool Player::IsLocalPlayer() const {
  return is_local_player_;
}

void Player::SetIsLocalPlayer(bool is_local_player) {
  is_local_player_ = is_local_player;
}

GameObjectType Player::GetType() const {
  return GameObjectType::kPlayer;
}

void Player::SetRadius(float radius) {
  SetWidth(radius * 2.f);
  SetHeight(radius * 2.f);
}

float Player::GetRadius() const {
  return GetWidth() / 2.f;
}

std::shared_ptr<GameObject> Player::Clone() const {
  return std::make_shared<Player>(*this);
}

const std::shared_ptr<Weapon>& Player::GetWeapon() const {
  return weapon_;
}

int Player::GetLevel() const {
  return level_;
}

float Player::GetCurrentExp() const {
  return current_exp_;
}

void Player::SetLevel(int level) {
  level_ = level;
}

void Player::SetCurrentExp(float current_exp) {
  current_exp_ = current_exp;
}

void Player::IncreaseExperience(float experience_to_add) {
  current_exp_ += experience_to_add;
  while (current_exp_ >= Constants::kExpForLevel[level_ - 1]) {
    current_exp_ -= Constants::kExpForLevel[level_ - 1];
    level_++;
  }
}
