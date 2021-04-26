#include "player.h"

Player::Player(GameObjectId player_id) : Entity(player_id) {}

Player::Player(const Player& other) : Entity(other) {
  is_local_player_ = other.is_local_player_;
  weapon_type_ = other.weapon_type_;
  switch (weapon_type_) {
    case WeaponType::kAssaultRifle: {
      weapon_ = std::make_shared<AssaultRifle>(
          *(std::dynamic_pointer_cast<AssaultRifle>(other.weapon_)));
      break;
    }
    case WeaponType::kCrossbow: {
      weapon_ = std::make_shared<Crossbow>(
          *(std::dynamic_pointer_cast<Crossbow>(other.weapon_)));
      break;
    }
    case WeaponType::kMachineGun: {
      weapon_ = std::make_shared<MachineGun>(
          *(std::dynamic_pointer_cast<MachineGun>(other.weapon_)));
      break;
    }
    case WeaponType::kShotgun: {
      weapon_ = std::make_shared<Shotgun>(
          *(std::dynamic_pointer_cast<Shotgun>(other.weapon_)));
      break;
    }
  }
  current_exp_ = other.current_exp_;
  level_ = other.level_;
}

void Player::SetParams(std::vector<QVariant> params) {
  auto weapon_type = static_cast<WeaponType>(params.back().toInt());
  weapon_type_ = weapon_type;
  switch (weapon_type) {
    case WeaponType::kAssaultRifle: {
      weapon_ = std::make_shared<AssaultRifle>();
      break;
    }
    case WeaponType::kCrossbow: {
      weapon_ = std::make_shared<Crossbow>();
      break;
    }
    case WeaponType::kMachineGun: {
      weapon_ = std::make_shared<MachineGun>();
      break;
    }
    case WeaponType::kShotgun: {
      weapon_ = std::make_shared<Shotgun>();
      break;
    }
  }
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result = Entity::GetParams();
  result.emplace_back(static_cast<int>(weapon_type_));
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

  painter->drawText(text_rect, Qt::AlignCenter,
                    QString::number(this->GetLevel()));
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
