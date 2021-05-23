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
  QPointF translation(0.f, -2.f);
  painter->Translate(translation);
  float rect_width = 75.f;
  float rect_height = 14.f;
  auto cur_hp = static_cast<int>(std::round(this->GetHealthPoints()));
  if (is_disappearing_) {
    cur_hp = 0.f;
  }
  auto max_hp = static_cast<int>(std::round(this->GetMaxHealthPoints()));
  QString text = QString::number(cur_hp) + " / " +
      QString::number(max_hp);
  QFont font = painter->font();
  float factor =
      rect_width * 0.9f / painter->fontMetrics().horizontalAdvance(
          QString::number(max_hp) + "/ " + QString::number(max_hp));
  font.setPointSizeF(font.pointSizeF() * factor);
  painter->setFont(font);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);
  painter->drawText(text_rect, Qt::AlignCenter,
                    text);
  auto pen = painter->pen();
  QColor clr_back = Qt::gray;
  clr_back.setAlphaF(0.4f);
  painter->setPen(clr_back);
  painter->drawRect(-rect_width / 2.f, -rect_height / 2.f,
                    rect_width, rect_height);
  auto brush = painter->brush();
  QColor clr_main = Qt::darkGreen;
  clr_main.setAlphaF(0.4f);
  painter->setBrush(clr_main);
  float part = this->GetHealthPoints() / this->GetMaxHealthPoints();
  float width = part * rect_width;
  painter->drawRect(-rect_width / 2.f, -rect_height / 2.f,
                    width, rect_height);
  painter->setBrush(brush);
  painter->setPen(pen);
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
  TickHealthPoints(delta_time);
  MovableObject::OnTick(delta_time);
}

void Entity::TickHealthPoints(int delta_time) {
  health_points_ = std::min(
      health_points_ + health_regen_rate_ * static_cast<float>(delta_time),
      max_health_points_);
}

void Entity::DrawLevel(Painter* painter) const {
  painter->save();
  painter->setOpacity(this->GetOpacity());
  QPointF translation(0.f,  -3.f);
  painter->Translate(translation);
  painter->setBrush(Qt::black);
  painter->drawEllipse(QPointF(), 10.f, 10.f);
  float rect_width = 75.f;
  float rect_height = 14.f;
  QFont font = painter->font();
  font.setPointSizeF(10.f);
  painter->setFont(font);
  QPen pen(Constants::Painter::kLevelColor);
  painter->setPen(pen);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);

  painter->drawText(text_rect, Qt::AlignCenter,
                    QString::number(this->GetLevel()));
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

void Entity::UpdateAnimationState(bool restart) {
  if (!is_disappearing_ && velocity_.length() > Math::kEps) {
    this->SetAnimationState(AnimationState::kMove, restart);
  } else {
    this->SetAnimationState(AnimationState::kIdle, restart);
  }
}
