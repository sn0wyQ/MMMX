#include "entity.h"

Entity::Entity(GameObjectId id) :
    MovableObject(id),
    opacity_emulator_(Constants::kOpacityChangeSpeed) {
  opacity_emulator_.SetStopOnMax(true);
  opacity_emulator_.SetStopOnMin(true);
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetPath(0.f, 1.f);
}

Entity::Entity(const Entity& other) :
    MovableObject(other),
    opacity_emulator_(other.opacity_emulator_) {
  fov_radius_ = other.fov_radius_;
  health_points_ = other.health_points_;
  health_regen_rate_ = other.health_regen_rate_;
  max_health_points_ = other.max_health_points_;
  level_ = other.level_;
}

void Entity::SetParams(std::vector<QVariant> params) {
  SetMaxHealthPoints(params.back().toFloat());
  params.pop_back();
  SetHealthRegenRate(params.back().toFloat());
  params.pop_back();
  SetHealthPoints(params.back().toFloat());
  params.pop_back();
  SetFovRadius(params.back().toFloat());
  params.pop_back();
  MovableObject::SetParams(params);
}

std::vector<QVariant> Entity::GetParams() const {
  std::vector<QVariant> result = MovableObject::GetParams();
  result.emplace_back(GetFovRadius());
  result.emplace_back(GetHealthPoints());
  result.emplace_back(GetHealthRegenRate());
  result.emplace_back(GetMaxHealthPoints());
  return result;
}

float Entity::GetFovRadius() const {
  return fov_radius_;
}

void Entity::SetFovRadius(float fov_radius) {
  fov_radius_ = fov_radius;
}

void Entity::SetHealthPoints(float health_points) {
  health_points_ = health_points;
  if (!this->IsAlive()) {
    this->SetDisappearing();
  }
}

float Entity::GetHealthPoints() const {
  return health_points_;
}

void Entity::SetMaxHealthPoints(float max_health_points) {
  max_health_points_ = max_health_points;
}

float Entity::GetMaxHealthPoints() const {
  return max_health_points_;
}

int Entity::GetLevel() const {
  return level_;
}

void Entity::SetLevel(int level) {
  level_ = level;
}

bool Entity::IsEntity() const {
  return true;
}

std::shared_ptr<GameObject> Entity::Clone() const {
  return std::make_shared<Entity>(*this);
}

void Entity::DrawHealthBar(Painter* painter) const {
  painter->save();
  painter->setOpacity(this->GetOpacity());
  painter->Translate(QPointF(-2.5f, -2.5f));
  QRectF rect(0, 0, 75, 14);

  painter->setBrush(Qt::gray);
  painter->setPen(Qt::transparent);
  painter->drawRoundedRect(rect, 10, 10);

  float hp_ratio = this->GetHealthPoints() / this->GetMaxHealthPoints();
  if (is_disappearing_) {
    hp_ratio = 0.f;
  }
  auto color = Constants::GetHealthPointsColor(hp_ratio);
  color.setAlphaF(0.8f);
  painter->setBrush(color);

  painter->setPen(Qt::transparent);
  auto painter_clip = painter->clipRegion();
  painter->setClipRegion(painter_clip.intersected(
      QRect(0, 0, rect.width() * hp_ratio, rect.height())));
  painter->drawRoundedRect(rect, 10, 10);
  painter->setClipRegion(painter_clip);

  painter->setBrush(Qt::transparent);
  painter->setPen(QPen(Qt::black, 1.5f));
  painter->drawRoundedRect(rect, 10, 10);

  painter->restore();
}

void Entity::Revive(QPointF point_to_spawn) {
  SetPosition(point_to_spawn);
  SetHealthPoints(GetMaxHealthPoints());
  this->SetAppearing();
}

float Entity::GetHealthRegenRate() const {
  return health_regen_rate_;
}

float Entity::GetExpIncrementForKill() const {
  return 0;
}

void Entity::SetHealthRegenRate(float health_regen_rate) {
  health_regen_rate_ = health_regen_rate;
}

void Entity::OnTick(int delta_time) {
  if (!IsAlive()) {
    return;
  }
  MovableObject::OnTick(delta_time);
  TickHealthPoints(delta_time);
}

void Entity::TickHealthPoints(int delta_time) {
  health_points_ = std::min(
      health_points_ + health_regen_rate_ * static_cast<float>(delta_time),
      max_health_points_);
}

void Entity::DrawLevel(Painter* painter) const {
  painter->save();
  painter->setOpacity(this->GetOpacity());
  QPointF translation(-3.f, -2.8f);
  painter->Translate(translation);
  painter->setBrush(Qt::black);
  QPixmap pixmap("../Res/Icons/level_arrow.png");
  QRectF rect(0, 0, 30, 25);
  painter->drawPixmap(rect, pixmap,
                      QRectF(0, 0, pixmap.width(), pixmap.height()));
  QFont font = painter->font();
  font.setPointSizeF(10.f);
  font.setBold(true);
  painter->setFont(font);
  QPen pen(Constants::Painter::kLevelColor);
  painter->setPen(pen);

  painter->drawText(rect, Qt::AlignCenter, QString::number(this->GetLevel()));
  painter->restore();
}

bool Entity::IsAlive() const {
  return GetHealthPoints() > 0;
}

float Entity::GetOpacity() const {
  return opacity_emulator_.GetCurrentValue();
}

void Entity::SetAppearing() {
  opacity_emulator_.SetCurrentValue(0.f);
  opacity_emulator_.SetPath(0.f, 1.f);
  is_disappearing_ = false;
}

void Entity::SetDisappearing() {
  opacity_emulator_.SetCurrentValue(1.f);
  opacity_emulator_.SetPath(1.f, 0.f);
  is_disappearing_ = true;
}
