#include "single_room_info.h"

SingleRoomInfo::SingleRoomInfo(AbstractRoomsInfoList* parent,
                               std::shared_ptr<ClientGameModel> model,
                               RoomInfo room_info)
    : QWidget(dynamic_cast<QWidget*>(parent)),
      parent_(parent),
      model_(std::move(model)),
      room_info_(room_info) {
  label_ = new QLabel(this);
  label_->setAlignment(Qt::AlignCenter);

  this->UpdateInfo(std::move(room_info_));
}

void SingleRoomInfo::UpdateInfo(const RoomInfo& room_info) {
  room_info_ = room_info;
  QString text = QString("%1 (ID: %2) | Owner ID: %3 | Max Players: %4")
                   .arg(room_info_.name)
                   .arg(room_info_.id)
                   .arg(room_info_.owners_client_id == Constants::kNullClientId
                        ? "[SERVER]"
                          : QString::number(room_info_.owners_client_id))
                   .arg(room_info_.max_clients);

  label_->setText((room_info_.is_public ? "[PUBLIC] " : "[PRIVATE] ")
                  + text);
}

void SingleRoomInfo::mouseMoveEvent(QMouseEvent* mouse_event) {
  if (mouse_event->type() == QMouseEvent::MouseButtonPress) {
    parent_->SelectRoom(room_info_.id);
  }
}

void SingleRoomInfo::resizeEvent(QResizeEvent* event) {
  label_->resize(event->size());
}
