#include "reloading_field.h"

ReloadingField::ReloadingField(QWidget* parent,
                               std::shared_ptr<ClientGameModel> model) :
    QWidget(parent), model_{std::move(model)}{
  this->move(Constants::ReloadingField::kDefaultPos);
  this->resize(Constants::ReloadingField::kDefaultSize);
}

void ReloadingField::paintEvent(QPaintEvent* paint_event) {
  if (!model_->IsLocalPlayerSet()) {
    return;
  }
  QPainter painter(this);

  this->DrawInPercents(&painter);
  this->DrawReloadingField(&painter);
}

void ReloadingField::DrawReloadingField(QPainter* painter) {
  painter->save();

  for (int i = 0; i < 5; i++) {
    QPolygonF reloading_polygon = Constants::ReloadingField::reloading_picture;
    for (int j = 0; j < 5; j++) {
      reloading_polygon[j] = QPointF(reloading_polygon[j].toPoint().x(), reloading_polygon[j].toPoint().y() + i * 10);
    }
    painter->drawPolygon(reloading_polygon);
  }

  painter->restore();
}

void ReloadingField::DrawInPercents(QPainter* painter) {
  painter->save();
  auto local_player = model_->GetLocalPlayer();
  // if (model_->)
  int clip_size = local_player->GetWeapon()->GetClipSize();
  int bullets_in_clip = local_player->GetWeapon()->GetCurrentBulletsInClip();
  painter->setBrush(Qt::blue);
  painter->drawRect(8, 90, 62, 50 * bullets_in_clip / clip_size);
  painter->restore();
}

void ReloadingField::resizeEvent(QResizeEvent*) {
  // this->move(15, 15);
  // this->resize(20, 20);
}
