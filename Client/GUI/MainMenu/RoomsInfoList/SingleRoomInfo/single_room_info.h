#ifndef SINGLE_ROOM_INFO_H_
#define SINGLE_ROOM_INFO_H_

#include <memory>
#include <utility>

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWidget>

#include "Client/GUI/MainMenu/RoomsInfoList/abstract_rooms_info_list.h"
#include "Client/client_game_model.h"
#include "Server/Room/room_info.h"

class SingleRoomInfo : public QWidget {
 public:
  SingleRoomInfo(AbstractRoomsInfoList* parent,
                 std::shared_ptr<ClientGameModel> model,
                 RoomInfo room_info);

  void UpdateInfo(const RoomInfo& room_info);

  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  AbstractRoomsInfoList* parent_;
  std::shared_ptr<ClientGameModel> model_;

  RoomInfo room_info_;

  QLabel* label_;
};

#endif  // SINGLE_ROOM_INFO_H_
