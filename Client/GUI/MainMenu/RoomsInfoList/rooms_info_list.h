#ifndef ROOMS_INFO_H_
#define ROOMS_INFO_H_

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <QList>
#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "Client/GUI/MainMenu/RoomsInfoList/SingleRoomInfo/single_room_info.h"
#include "Client/client_game_model.h"
#include "Server/Room/room_info.h"

class RoomsInfoList : public QWidget {
 public:
  RoomsInfoList(QWidget* parent, std::shared_ptr<ClientGameModel> model);

  void UpdateRoomsInfoList();
  void SelectRoom(RoomId room_id);

  void resizeEvent(QResizeEvent* event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;

  RoomId selected_room_id_{Constants::kNullRoomId};
  std::map<RoomId, SingleRoomInfo*> rooms_;

  QScrollArea* scroll_area_;
  QVBoxLayout* layout_;
};

#endif  // ROOMS_INFO_H_
