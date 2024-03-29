#ifndef GUI_MAINMENU_ROOMSINFOLIST_ROOMS_INFO_LIST_H_
#define GUI_MAINMENU_ROOMSINFOLIST_ROOMS_INFO_LIST_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <QList>
#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "GUI/MainMenu/RoomsInfoList/SingleRoomInfo/single_room_info.h"
#include "GUI/MainMenu/RoomsInfoList/abstract_rooms_info_list.h"
#include "Client/client_game_model.h"
#include "Room/room_info.h"

class RoomsInfoList : public AbstractRoomsInfoList {
  Q_OBJECT

 public:
  RoomsInfoList(QWidget* parent, std::shared_ptr<ClientGameModel> model);

  void UpdateRoomsInfoList();

  RoomId GetSelectedRoom() const override;
  void SelectRoom(RoomId room_id) override;

  void resizeEvent(QResizeEvent* event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;

  RoomId selected_room_id_{Constants::kNullRoomId};
  std::map<RoomId, SingleRoomInfo*> rooms_;

  QScrollArea* scroll_area_;
  QVBoxLayout* layout_;
};

#endif  // GUI_MAINMENU_ROOMSINFOLIST_ROOMS_INFO_LIST_H_
