#include "entity.h"

Entity::Entity(GameObjectId id) : MovableObject(id) {}

Entity::Entity(const Entity& other) : MovableObject(other) {
  fov_radius_ = other.fov_radius_;
  health_points_ = other.health_points_;
  health_regen_speed_ = other.health_regen_speed_;
  max_health_points_ = other.max_health_points_;
}

void Entity::SetParams(std::vector<QVariant> params) {
  SetMaxHealthPoints(params.back().toFloat());
  params.pop_back();
  SetHealthRegenSpeed(params.back().toFloat());
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
  result.emplace_back(GetHealthRegenSpeed());
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
  // die if <= 0
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
  QPointF translation(0.f, -GetHeight() * 0.7f);
  painter->Translate(translation);
  float rect_width = 2.5f;
  float rect_height = 0.6f;
  QFont font{};
  font.setPointSizeF(painter->GetScaledFloat(0.3f));
  painter->setFont(font);
  QRectF text_rect(-rect_width / 2.f, -rect_height / 2.f,
                   rect_width, rect_height);
  auto cur_hp = static_cast<int>(this->GetHealthPoints());
  auto max_hp = static_cast<int>(this->GetMaxHealthPoints());
  painter->DrawText(text_rect, Qt::AlignCenter,
                    QString::number(cur_hp) + " / " +
                    QString::number(max_hp));
  auto pen = painter->pen();
  QColor clr_back = Qt::gray;
  clr_back.setAlphaF(0.4f);
  painter->setPen(clr_back);
  painter->DrawRect(-rect_width / 2.f, -rect_height / 2.f,
                    rect_width, rect_height);
  auto brush = painter->brush();
  QColor clr_main = Qt::darkGreen;
  clr_main.setAlphaF(0.4f);
  painter->setBrush(clr_main);
  float part = this->GetHealthPoints() / this->GetMaxHealthPoints();
  float width = part * rect_width;
  painter->DrawRect(-rect_width / 2.f, -rect_height / 2.f,
                    width, rect_height);
  painter->setBrush(brush);
  painter->setPen(pen);
  painter->Translate(-translation);
}

void Entity::Revive() {
  SetPosition(GetSpawnPosition());
  SetHealthPoints(GetMaxHealthPoints());
}

void Entity::SetSpawnPosition(QPointF spawn_position) {
  spawn_position_ = spawn_position;
}

QPointF Entity::GetSpawnPosition() const {
  return spawn_position_;
}

float Entity::GetHealthRegenSpeed() const {
  return health_regen_speed_;
}

void Entity::SetHealthRegenSpeed(float health_regen_speed) {
  health_regen_speed_ = health_regen_speed;
}

void Entity::OnTick(int delta_tick) {
  health_points_ = std::min(
      health_points_ + health_regen_speed_ * static_cast<float>(delta_tick),
      max_health_points_);
  MovableObject::OnTick(delta_tick);
}
