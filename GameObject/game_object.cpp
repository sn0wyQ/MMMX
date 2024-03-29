#include "game_object.h"

AnimationsHolder GameObject::animations_holder_;

GameObject::GameObject(GameObjectId id)
  : id_(id) {}

GameObject::GameObject(const GameObject& other) {
  id_ = other.id_;
  SetAnimation(other.animation_->GetType());
  if (rigid_body_ == nullptr) {
    switch (other.GetRigidBody()->GetType()) {
      case RigidBodyType::kCircle:
        rigid_body_ = std::make_shared<RigidBodyCircle>();
        break;
      case RigidBodyType::kRectangle:
        rigid_body_ = std::make_shared<RigidBodyRectangle>();
        break;
      default:
        qWarning() << "Invalid rigid body type";
        break;
    }
  }
  rigid_body_->SetHeight(other.rigid_body_->GetHeight());
  rigid_body_->SetWidth(other.rigid_body_->GetWidth());
  SetHeight(other.GetHeight());
  SetWidth(other.GetWidth());
  SetRotation(other.GetRotation());
  SetY(other.GetY());
  SetX(other.GetX());
  updated_time_ = other.updated_time_;
  is_need_to_delete_ = other.is_need_to_delete_;
  created_time_ = other.created_time_;
  is_interpolated_once_ = other.is_interpolated_once_;
}

void GameObject::SetParams(std::vector<QVariant> params) {
  auto animation_type = static_cast<AnimationType>(params.back().toInt());
  SetAnimation(animation_type);
  params.pop_back();
  float rigid_body_height = params.back().toFloat();
  params.pop_back();
  float rigid_body_width = params.back().toFloat();
  params.pop_back();
  auto rigid_body_type = static_cast<RigidBodyType>(params.back().toInt());
  if (!rigid_body_) {
    switch (rigid_body_type) {
      case RigidBodyType::kCircle:
        rigid_body_ = std::make_shared<RigidBodyCircle>();
        break;
      case RigidBodyType::kRectangle:
        rigid_body_ = std::make_shared<RigidBodyRectangle>();
        break;
      default:
        qWarning() << "[GAME OBJECT] Invalid rigid body type"
                   << params.back().toInt();
        break;
    }
  }
  rigid_body_->SetHeight(rigid_body_height);
  rigid_body_->SetWidth(rigid_body_width);
  params.pop_back();
  SetHeight(params.back().toFloat());
  params.pop_back();
  SetWidth(params.back().toFloat());
  params.pop_back();
  SetRotation(params.back().toFloat());
  params.pop_back();
  SetY(params.back().toFloat());
  params.pop_back();
  SetX(params.back().toFloat());
  params.pop_back();
}

std::vector<QVariant> GameObject::GetParams() const {
  std::vector<QVariant> result;
  result.emplace_back(GetX());
  result.emplace_back(GetY());
  result.emplace_back(GetRotation());
  result.emplace_back(GetWidth());
  result.emplace_back(GetHeight());
  result.emplace_back(static_cast<int>(rigid_body_->GetType()));
  result.emplace_back(rigid_body_->GetWidth());
  result.emplace_back(rigid_body_->GetHeight());
  result.emplace_back(static_cast<int>(animation_->GetType()));
  return result;
}

void GameObject::Draw(Painter* painter) const {
  painter->save();
  painter->Translate(position_);
  painter->RotateCounterClockWise(rotation_);
  painter->setOpacity(this->GetOpacity());
  if (!animation_ || animation_->GetType() == AnimationType::kNone) {
    this->DrawRelatively(painter);
  } else {
    animation_->RenderFrame(painter, GetWidth(), GetHeight());
  }
  if (Constants::kRigidBodyShow) {
    painter->setPen(Qt::red);
    rigid_body_->Draw(painter);
  }
  if (this->GetType() == GameObjectType::kMapBorder) {
    QPen pen(Qt::red);
    pen.setWidth(4);
    float w = rigid_body_->GetWidth();
    float h = rigid_body_->GetHeight();
    painter->setPen(pen);
    painter->DrawRect(-w / 2.f, -h / 2.f, w, h);
  }
  painter->restore();
}

GameObjectId GameObject::GetId() const {
  return id_;
}

QPointF GameObject::GetPosition() const {
  return position_;
}

void GameObject::SetPosition(QPointF point) {
  position_ = point;
}

void GameObject::SetX(float x) {
  position_.setX(x);
}

void GameObject::SetY(float y) {
  position_.setY(y);
}

float GameObject::GetX() const {
  return static_cast<float>(position_.x());
}

float GameObject::GetY() const {
  return static_cast<float>(position_.y());
}

float GameObject::GetRotation() const {
  return rotation_;
}

void GameObject::SetRotation(float rotation) {
  rotation_ = rotation;
}

std::shared_ptr<RigidBody> GameObject::GetRigidBody() const {
  return rigid_body_;
}

float GameObject::GetFrictionForce() const {
  return IntersectChecker::kDefaultFrictionForce;
}

float GameObject::GetWidth() const {
  return width_;
}

float GameObject::GetHeight() const {
  return height_;
}

void GameObject::SetWidth(float width) {
  width_ = width;
}

void GameObject::SetHeight(float height) {
  height_ = height;
}

QRectF GameObject::GetBoundingRect() const {
  QPointF offset(this->GetWidth(), this->GetHeight());
  return QRectF(QPointF(this->GetPosition() - offset / 2.f),
                QPointF(this->GetPosition() + offset / 2.f));
}

bool GameObject::IsMovable() const {
  return false;
}

bool GameObject::IsNeedToDelete() const {
  return is_need_to_delete_;
}

void GameObject::SetIsNeedToDelete(bool is_need_to_delete) {
  is_need_to_delete_ = is_need_to_delete;
}

GameObjectType GameObject::GetType() const {
  return GameObjectType::kGameObject;
}

bool GameObject::IsFilteredByFov() const {
  return false;
}

std::shared_ptr<Animation> GameObject::GetAnimation() {
  return animation_;
}

void GameObject::SetAnimation(AnimationType animation_type) {
  if (animation_->GetType() != AnimationType::kNone) {
    return;
  }
#ifdef MMMX_SERVER
  animation_ = std::make_shared<Animation>(animation_type);
#else
  animation_ = animations_holder_.GetAnimation(animation_type);
#endif  // MMMX_SERVER
}

void GameObject::SetAnimationState(AnimationState animation_state,
                                   bool restart) {
  if (!animation_) {
    return;
  }
  animation_->SetAnimationState(animation_state, restart);
}

AnimationsHolder& GameObject::GetAnimationsHolder() {
  return animations_holder_;
}

std::shared_ptr<GameObject> GameObject::Clone() const {
  return std::make_shared<GameObject>(*this);
}

void GameObject::SetUpdatedTime(int64_t updated_time) {
  updated_time_ = updated_time;
}

int64_t GameObject::GetUpdatedTime() const {
  return updated_time_;
}

bool GameObject::IsEntity() const {
  return false;
}

float GameObject::GetRigidBodyBoundingCircleRadius() const {
  if (GetRigidBody()->GetType() == RigidBodyType::kCircle) {
    return std::dynamic_pointer_cast<RigidBodyCircle>(
        GetRigidBody())->GetRadius();
  }
  return Math::DistanceBetweenPoints(
      QPointF(), QPointF(this->GetRigidBody()->GetWidth() / 2.f,
                         this->GetRigidBody()->GetHeight() / 2.f));
}

int64_t GameObject::GetCreatedTime() const {
  return created_time_;
}

void GameObject::SetCreatedTime(int64_t created_time) {
  created_time_ = created_time;
}

bool GameObject::IsInterpolatedOnce() const {
  return is_interpolated_once_;
}

void GameObject::SetIsInterpolatedOnce(bool is_interpolated_once) {
  is_interpolated_once_ = is_interpolated_once;
}

bool GameObject::IsAlive() const {
  return true;
}

bool GameObject::IsNeedToDraw() const {
  if (this->IsInterpolatedOnce()) {
    return false;
  }
  return true;
}

float GameObject::GetOpacity() const {
  return opacity_;
}

void GameObject::SetAppearing() {
  opacity_ = 1.f;
}

void GameObject::SetDisappearing() {
  opacity_ = 0.f;
}

bool GameObject::Intersects(const Math::Line& line) {
  switch (rigid_body_->GetType()) {
    case RigidBodyType::kCircle:
      return !Math::GetCircleAndLineIntersections(
          this->GetPosition(), this->GetRigidBodyBoundingCircleRadius(),
          line).empty();

    case RigidBodyType::kRectangle: {
      QPointF offset(rigid_body_->GetWidth() / 2.f,
                     rigid_body_->GetHeight() / 2.f);
      return !Math::GetRectWithLineIntersections(
          QRectF(this->position_ - offset, this->position_ + offset),
          line, this->GetRotation()).empty();
    }

    default:
      qWarning() << "Invalid rigid body type";
      return false;
  }
}
