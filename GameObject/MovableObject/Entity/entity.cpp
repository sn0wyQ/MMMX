#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

Entity::Entity(const Entity& other) : MovableObject(other) {
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

void Entity::DrawHealthBar(Painter* painter) {
  painter->save();
  QPointF translation(0.f, -2.f);
  painter->Translate(translation);
  float rect_width = 75.f;
  float rect_height = 14.f;
  auto cur_hp = static_cast<int>(this->GetHealthPoints());
  auto max_hp = static_cast<int>(this->GetMaxHealthPoints());
  QString text = QString::number(cur_hp) + " / " +
      QString::number(max_hp);
  QFont font = painter->font();
  float factor =
      rect_width * 0.9f / painter->fontMetrics().horizontalAdvance(
          QString::number(max_hp) + "/" + QString::number(max_hp));
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
  painter->Translate(-translation);
  painter->restore();
}

void Entity::Revive(QPointF point_to_spawn) {
  SetIsVisible(true);
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

void Entity::DrawLevel(Painter* painter) {
  painter->save();
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
  painter->Translate(-translation);
  painter->restore();
}

bool Entity::IsAlive() const {
  return GetHealthPoints() > 0;
}
