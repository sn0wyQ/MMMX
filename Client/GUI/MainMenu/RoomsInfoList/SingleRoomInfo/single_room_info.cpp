#include "single_room_info.h"

SingleRoomInfo::SingleRoomInfo(QWidget* parent,
                               std::shared_ptr<ClientGameModel> model,
                               const RoomInfo& room_info)
    : QWidget(parent), model_(std::move(model)) {
  this->setMouseTracking(false);

  label_ = new QLabel(this);
  label_->setAlignment(Qt::AlignCenter);

  this->UpdateInfo(room_info);
}

void SingleRoomInfo::UpdateInfo(const RoomInfo& room_info) {
  QString text = QString("%1 (ID: %2) | Owner ID: %3 | Max Players: %4")
                   .arg(room_info.name)
                   .arg(room_info.id)
                   .arg(room_info.owners_client_id == Constants::kNullClientId
                        ? "[SERVER]"
                          : QString::number(room_info.owners_client_id))
                   .arg(room_info.max_clients);

  label_->setText((room_info.is_public ? "[PUBLIC] " : "[PRIVATE] ")
                  + text);
}

void SingleRoomInfo::resizeEvent(QResizeEvent* event) {
  label_->resize(event->size());
}
