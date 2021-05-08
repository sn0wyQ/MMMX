#include "rooms_info_list.h"

RoomsInfoList::RoomsInfoList(QWidget* parent,
                             std::shared_ptr<ClientGameModel> model)
    : QWidget(parent), model_(std::move(model)) {
  this->setMouseTracking(false);

  layout_ = new QVBoxLayout(this);
  this->setLayout(layout_);

  scroll_area_ = new QScrollArea(this);
  scroll_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scroll_area_->setWidgetResizable(true);
}

void RoomsInfoList::UpdateRoomsInfoList() {
  std::set<RoomId> updated_rooms;
  for (const auto& room_info : model_->GetRoomsInfo()) {
    auto room_iter = rooms_.find(room_info.id);
    if (room_iter != rooms_.end()) {
      room_iter->second->UpdateInfo(room_info);
    } else {
      auto* new_room_info_widget = new SingleRoomInfo(this, model_, room_info);
      layout_->addWidget(new_room_info_widget);
      rooms_.insert({room_info.id, new_room_info_widget});
    }
    updated_rooms.insert(room_info.id);
  }

  // Removing rooms that are no longer available
  for (const auto& [room_id, widget_ptr] : rooms_) {
    if (updated_rooms.find(room_id) == updated_rooms.end()) {
      layout_->removeWidget(widget_ptr);
      widget_ptr->deleteLater();
    }
  }
}

RoomId RoomsInfoList::GetSelectedRoom() const {
  return selected_room_id_;
}

void RoomsInfoList::SelectRoom(RoomId room_id) {
  selected_room_id_ = room_id;
}

void RoomsInfoList::resizeEvent(QResizeEvent* event) {
  scroll_area_->resize(event->size());
}
