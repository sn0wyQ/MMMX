#include "creep.h"

Creep::Creep(GameObjectId player_id) : Entity(player_id) {}

void Creep::DrawRelatively(Painter* painter) {
  painter->DrawEllipse(QPointF(),
                       this->GetWidth() / 2 - 0.1,
                       this->GetHeight() / 2 - 0.1);
}

GameObjectType Creep::GetType() const {
  return GameObjectType::kCreep;
}

void Creep::SetParams(std::vector<QVariant> params) {
  this->SetExpIncrementForKill(params.back().toFloat());
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Creep::GetParams() const {
  auto params = Entity::GetParams();
  params.emplace_back(this->GetExpIncrementForKill());
  return params;
}

void Creep::SetExpIncrementForKill(float exp_increment) {
  exp_increment_ = exp_increment;
}

float Creep::GetExpIncrementForKill() const {
  return exp_increment_;
}

float& Creep::GetMutableExpIncrementForKill() {
  return exp_increment_;
}

std::shared_ptr<GameObject> Creep::Clone() const {
  return std::make_shared<Creep>(*this);
}
