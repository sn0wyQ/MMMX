#ifndef ABSTRACT_ROOMS_INFO_LIST_H_
#define ABSTRACT_ROOMS_INFO_LIST_H_

#include <QWidget>

#include "constants.h"

class AbstractRoomsInfoList : public QWidget {
 public:
  explicit AbstractRoomsInfoList(QWidget* parent) : QWidget(parent) {}

  virtual RoomId GetSelectedRoom() const = 0;
  virtual void SelectRoom(RoomId room_id) = 0;
};

#endif  // ABSTRACT_ROOMS_INFO_LIST_H_
