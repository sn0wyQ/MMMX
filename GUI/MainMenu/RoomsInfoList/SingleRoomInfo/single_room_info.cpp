#include "single_room_info.h"

SingleRoomInfo::SingleRoomInfo(AbstractRoomsInfoList* parent,
                               std::shared_ptr<ClientGameModel> model,
                               RoomInfo room_info)
    : QWidget(dynamic_cast<QWidget*>(parent)),
      parent_(parent),
      model_(std::move(model)),
      room_info_(std::move(room_info)) {
  label_ = new QLabel(this);
  label_->setAlignment(Qt::AlignCenter);

  this->SetRoomInfo(room_info_);
}

QString SingleRoomInfo::ParseRoomInfo() const {
  QString text = QString("%1 (ID: %2) | Owner ID: %3 | Max Players: %4")
      .arg(room_info_.name)
      .arg(room_info_.id)
      .arg(room_info_.owners_client_id == Constants::kNullClientId
           ? "[SERVER]"
           : QString::number(room_info_.owners_client_id))
      .arg(room_info_.max_clients);

  return (parent_->GetSelectedRoom() == room_info_.id ? "----> " : QString())
         + (room_info_.is_public ? "[PUBLIC] " : "[PRIVATE] ") + text;
}

void SingleRoomInfo::SetRoomInfo(const RoomInfo& room_info) {
  room_info_ = room_info;
  label_->setText(ParseRoomInfo());
}

RoomInfo SingleRoomInfo::GetRoomInfo() const {
  return room_info_;
}

void SingleRoomInfo::mousePressEvent(QMouseEvent* mouse_event) {
  if (mouse_event->button() == Qt::LeftButton) {
    parent_->SelectRoom(room_info_.id);
  }
}

void SingleRoomInfo::paintEvent(QPaintEvent* event) {
  label_->setText(ParseRoomInfo());
}

void SingleRoomInfo::resizeEvent(QResizeEvent* event) {
  label_->resize(event->size());
}
