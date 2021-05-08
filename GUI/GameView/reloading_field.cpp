#include "reloading_field.h"

ReloadingField::ReloadingField(QWidget* parent,
                               std::shared_ptr<ClientController> controller) :
    QWidget(parent), controller_{std::move(controller)} {
  this->move(Constants::ReloadingField::kDefaultPos);
  this->resize(Constants::ReloadingField::kDefaultSize);
}

void ReloadingField::paintEvent(QPaintEvent* paint_event) {
  if (!controller_->GetModel()->IsLocalPlayerSet()) {
    return;
  }
  QPainter painter(this);

  this->DrawInPercents(&painter);
  this->DrawReloadingField(&painter);
}

void ReloadingField::DrawReloadingField(QPainter* painter) {
  painter->save();

  for (int i = 0; i < Constants::ReloadingField::kBullets; i++) {
    QPolygonF reloading_polygon = Constants::ReloadingField::kBulletPicture;
    for (int j = 0; j < Constants::ReloadingField::kNodes; j++) {
      reloading_polygon[j] = QPointF(reloading_polygon[j].toPoint().x(),
                               reloading_polygon[j].toPoint().y() + i * 10);
    }
    painter->drawPolygon(reloading_polygon);
  }

  painter->restore();
}

void ReloadingField::DrawInPercents(QPainter* painter) {
  painter->save();
  auto local_player_weapon =
      controller_->GetModel()->GetLocalPlayer()->GetWeapon();

  int64_t last_time_pressed_reload =
      local_player_weapon->GetLastTimePressedReload();
  int64_t reloading_time = local_player_weapon->GetReloadingTime();
  int64_t cur_time = controller_->GetCurrentServerTime();
  int64_t delta_time = cur_time - last_time_pressed_reload;
  int width = Constants::ReloadingField::kDefaultWidth;
  painter->setClipRegion(Constants::ReloadingField::kBulletsRegion);

  if (delta_time <= reloading_time) {
    painter->setBrush(Qt::darkBlue);
    painter->drawRect(0, 0, width, 50 * delta_time / reloading_time);
  } else {
    int clip_size = local_player_weapon->GetClipSize();
    int bullets_in_clip = local_player_weapon->GetCurrentBulletsInClip();
    painter->setBrush(Qt::black);
    painter->drawRect(0, 0, width, 50 * bullets_in_clip / clip_size);
  }
  painter->restore();
}

void ReloadingField::resizeEvent(QResizeEvent*) {
  this->move(Constants::ReloadingField::kDefaultPos);
  this->resize(Constants::ReloadingField::kDefaultSize);
}
