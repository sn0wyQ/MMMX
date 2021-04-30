#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

Entity::Entity(const Entity& other) : MovableObject(other) {
  fov_radius_ = other.fov_radius_;
  health_points_ = other.health_points_;
  health_regen_rate_ = other.health_regen_rate_;
  max_health_points_ = other.max_health_points_;
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

bool Entity::IsEntity() const {
  return true;
}

std::shared_ptr<GameObject> Entity::Clone() const {
  return std::make_shared<Entity>(*this);
}

void Entity::DrawHealthBar(Painter* painter) {
  QPointF translation(0.f, -GetHeight() / 2 - 1);
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
                    QString::number(cur_hp) + " / " +
                    QString::number(max_hp));
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
  painter->Translate(-translation);
}

void Entity::Revive(QPointF point_to_spawn) {
  SetPosition(point_to_spawn);
  SetHealthPoints(GetMaxHealthPoints());
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
  MovableObject::OnTick(delta_time);
  TickHealthPoints(delta_time);
}

void Entity::TickHealthPoints(int delta_time) {
  health_points_ = std::min(
      health_points_ + health_regen_rate_ * static_cast<float>(delta_time),
      max_health_points_);
}
