#include "player.h"

Player::Player(GameObjectId player_id)
    : Entity(player_id),
      leveling_points_(Constants::kLevelingCount) {
  free_leveling_points_ = 5;
}

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
    default: break;
  }
  current_exp_ = other.current_exp_;
  level_ = other.level_;
  free_leveling_points_ = other.free_leveling_points_;
  leveling_points_ = other.leveling_points_;
  need_to_send_leveling_points_ = other.need_to_send_leveling_points_;
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
    default: break;
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
  painter->save();
  QPointF translation(0.f, -2.5f);
  painter->Translate(translation);
  float rect_width = 75.f;
  float rect_height = 14.f;
  QFont font(Constants::Painter::kDefaultFont);
  font.setPointSizeF(20.f / GetHeight());
  painter->setFont(font);
  QPen pen(Constants::Painter::kLevelColor);
  painter->setPen(pen);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);

  painter->drawText(text_rect, Qt::AlignCenter,
                    QString::number(this->GetLevel()));
  painter->Translate(-translation);
  painter->restore();
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
  while (current_exp_ >= Constants::GetExpForLevel(level_)) {
    if (level_ + 1 > Constants::kMaxLevel) {
      break;
    }
    current_exp_ -= Constants::GetExpForLevel(level_);
    free_leveling_points_++;
    level_++;
  }
  current_exp_ = std::min(current_exp_, Constants::GetExpForLevel(level_));
}

int Player::GetFreeLevelingPoints() const {
  return free_leveling_points_;
}

const std::vector<int>& Player::GetLevelingPoints() const {
  return leveling_points_;
}

void Player::SetFreeLevelingPoints(int free_leveling_points) {
  free_leveling_points_ = free_leveling_points;
}

void Player::IncreaseLevelingPoint(int index) {
  switch (index) {
    case 0: {
      float part_to_set = GetHealthPoints() / GetMaxHealthPoints();
      SetMaxHealthPoints(GetMaxHealthPoints() * 1.5f);
      SetHealthPoints(GetMaxHealthPoints() * part_to_set);
      break;
    }
    case 1:
      SetHealthRegenRate(GetHealthRegenRate() * 2.1f);
      break;
    case 2:
      SetSpeedMultiplier(GetSpeedMultiplier() * 1.2f);
      break;
    case 3:
      SetFovRadius(GetFovRadius() * 1.4f);
      break;
    case 4:
      break;
    case 5:
      weapon_->SetBulletSpeed(weapon_->GetBulletSpeed() * 1.2f);
      break;
    case 6:
      weapon_->SetRateOfFire(
          static_cast<int>(
              static_cast<float>(weapon_->GetRateOfFire()) * 1.1f));
      break;
    case 7:
      weapon_->SetBulletRange(weapon_->GetBulletRange() * 1.3f);
      break;
    case 8:
      weapon_->SetBulletDamage(weapon_->GetBulletDamage() * 1.1f);
      break;
    case 9:
      break;
    default:
      break;
  }
  leveling_points_[index]++;
  need_to_send_leveling_points_ = true;
}

bool Player::IsNeedToSendLevelingPoints() const {
  return need_to_send_leveling_points_;
}

void Player::SetNeedToSendLevelingPoints(bool need_to_send_leveling_points) {
  need_to_send_leveling_points_ = need_to_send_leveling_points;
}
