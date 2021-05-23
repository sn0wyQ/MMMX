#ifndef CLIENT_GUI_MAINMENU_ROOMSINFOLIST_SINGLEROOMINFO_SINGLE_ROOM_INFO_H_
#define CLIENT_GUI_MAINMENU_ROOMSINFOLIST_SINGLEROOMINFO_SINGLE_ROOM_INFO_H_

#include <memory>
#include <utility>

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

#include "Client/GUI/MainMenu/RoomsInfoList/abstract_rooms_info_list.h"
#include "Client/client_game_model.h"
#include "Server/Room/room_info.h"

class SingleRoomInfo : public QWidget {
  Q_OBJECT

 public:
  SingleRoomInfo(AbstractRoomsInfoList* parent,
                 std::shared_ptr<ClientGameModel> model,
                 RoomInfo room_info);

  void SetRoomInfo(const RoomInfo& room_info);
  RoomInfo GetRoomInfo() const;

  void mousePressEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  QString ParseRoomInfo() const;

  AbstractRoomsInfoList* parent_;
  std::shared_ptr<ClientGameModel> model_;

  RoomInfo room_info_;

  QLabel* label_;
};

#endif  // CLIENT_GUI_MAINMENU_ROOMSINFOLIST_SINGLEROOMINFO_SINGLE_ROOM_INFO_H_
