#include "creep.h"

Creep::Creep(GameObjectId player_id) : Entity(player_id) {}

void Creep::DrawRelatively(Painter* painter) {
  painter->DrawEllipse(QPointF(),
                       this->GetWidth() / 2 - 0.1,
                       this->GetHeight() / 2 - 0.1);
}

void Creep::DrawLevel(Painter* painter) {
  QPointF translation(0.f, -this->GetHeight() / 2 - 2);
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

GameObjectType Creep::GetType() const {
  return GameObjectType::kCreep;
}

void Creep::SetParams(std::vector<QVariant> params) {
  this->SetExpIncrementForKill(params.back().toFloat());
  params.pop_back();
  this->SetLevel(params.back().toInt());
  params.pop_back();
  Entity::SetParams(params);
}

std::vector<QVariant> Creep::GetParams() const {
  auto params = Entity::GetParams();
  params.emplace_back(this->GetLevel());
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

int Creep::GetLevel() const {
  return level_;
}

void Creep::SetLevel(int level) {
  level_ = level;
}
