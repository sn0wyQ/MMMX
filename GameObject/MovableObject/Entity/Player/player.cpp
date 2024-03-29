#include "player.h"

Player::Player(GameObjectId player_id)
    : Entity(player_id),
      leveling_points_(static_cast<int>(LevelingSlots::SIZE)) {
  free_leveling_points_ = 3;
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

    default: {
      qWarning() << "Invalid weapon type";
      break;
    }
  }
  current_exp_ = other.current_exp_;
  free_leveling_points_ = other.free_leveling_points_;
  leveling_points_ = other.leveling_points_;
}

void Player::SetParams(std::vector<QVariant> params) {
  weapon_type_ = static_cast<WeaponType>(params.back().toInt());
  params.pop_back();
  switch (weapon_type_) {
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

    default: {
      qWarning() << "Invalid weapon type";
      break;
    }
  }
  Entity::SetParams(params);
}

std::vector<QVariant> Player::GetParams() const {
  std::vector<QVariant> result = Entity::GetParams();
  result.emplace_back(static_cast<int>(weapon_type_));
  return result;
}

void Player::DrawRelatively(Painter* painter) const {
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

float Player::GetCurrentExp() const {
  return current_exp_;
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

void Player::IncreaseLevelingPoint(LevelingSlots leveling_slot) {
  switch (leveling_slot) {
    case LevelingSlots::kMaxHp: {
      float part_to_set = GetHealthPoints() / GetMaxHealthPoints();
      SetMaxHealthPoints(GetMaxHealthPoints()
        * Constants::LevelingSlots::kMaxHp);
      SetHealthPoints(GetMaxHealthPoints() * part_to_set);
      break;
    }

    case LevelingSlots::kHealthRegenRate: {
      SetHealthRegenRate(GetHealthRegenRate()
      * Constants::LevelingSlots::kHealthRegenRate);
      break;
    }

    case LevelingSlots::kSpeed: {
      SetSpeedMultiplier(
          GetSpeedMultiplier() * Constants::LevelingSlots::kSpeed);
      break;
    }

    case LevelingSlots::kFovRadius: {
      SetFovRadius(GetFovRadius()
                       * Constants::LevelingSlots::kFovRadius);
      break;
    }

    case LevelingSlots::kAccuracy: {
      weapon_->SetAccuracy(
          weapon_->GetAccuracy() * Constants::LevelingSlots::kAccuracy);
      break;
    }

    case LevelingSlots::kBulletSpeed: {
      weapon_->SetBulletSpeed(weapon_->GetBulletSpeed()
                              * Constants::LevelingSlots::kBulletSpeed);
      break;
    }

    case LevelingSlots::kRateOfFire: {
      weapon_->SetRateOfFire(
          static_cast<int>(
              static_cast<float>(weapon_->GetRateOfFire())
                  * Constants::LevelingSlots::kRateOfFire));
      break;
    }

    case LevelingSlots::kBulletRange: {
      weapon_->SetBulletRange(weapon_->GetBulletRange()
                            * Constants::LevelingSlots::kBulletRange);
      break;
    }

    case LevelingSlots::kBulletDamage: {
      weapon_->SetBulletDamage(weapon_->GetBulletDamage()
                             * Constants::LevelingSlots::kBulletDamage);
      break;
    }

    case LevelingSlots::kReloadingTime: {
      weapon_->SetReloadingTime(
          static_cast<int>(
              static_cast<float>(weapon_->GetReloadingTime())
                  * Constants::LevelingSlots::kReloadingTime));
      break;
    }

    default:
      break;
  }
  leveling_points_[static_cast<int>(leveling_slot)]++;
  need_to_send_leveling_points_ = true;
}

bool Player::IsNeedToSendLevelingPoints() const {
  return need_to_send_leveling_points_;
}

void Player::SetNeedToSendLevelingPoints(bool need_to_send_leveling_points) {
  need_to_send_leveling_points_ = need_to_send_leveling_points;
}

float Player::GetExpIncrementForKill() const {
  return static_cast<float>(this->GetLevel()) * Constants::kExpMultiplier;
}

void Player::DrawNickname(Painter* painter,
                          const QString& nickname) const {
  painter->save();
  painter->setOpacity(opacity_emulator_.GetCurrentValue());
  QPointF translation(0.f, -4.8f);
  painter->Translate(translation);
  painter->setBrush(Qt::red);
  float rect_width = 120.f;
  float rect_height = 16.f;
  QFont font = Constants::Painter::kBoldFont;
  font.setPointSizeF(12.f);
  painter->setFont(font);
  QPen pen(Constants::Painter::kNicknameColor);
  painter->setPen(pen);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);

  painter->drawText(text_rect, Qt::AlignCenter,
                    nickname);
  painter->restore();
}

void Player::Revive(QPointF point_to_spawn) {
  Entity::Revive(point_to_spawn);
  this->GetWeapon()->SetCurrentBulletsInClip(
      this->GetWeapon()->GetClipSize());
}

bool Player::IsPlayer() const {
  return true;
}
