#ifndef CLIENT_GUI_MAINMENU_ROOMSINFOLIST_ABSTRACT_ROOMS_INFO_LIST_H_
#define CLIENT_GUI_MAINMENU_ROOMSINFOLIST_ABSTRACT_ROOMS_INFO_LIST_H_

#include <QWidget>

#include "Constants/constants.h"

class AbstractRoomsInfoList : public QWidget {
 public:
  explicit AbstractRoomsInfoList(QWidget* parent) : QWidget(parent) {}

  virtual RoomId GetSelectedRoom() const = 0;
  virtual void SelectRoom(RoomId room_id) = 0;
};

#endif  // CLIENT_GUI_MAINMENU_ROOMSINFOLIST_ABSTRACT_ROOMS_INFO_LIST_H_
