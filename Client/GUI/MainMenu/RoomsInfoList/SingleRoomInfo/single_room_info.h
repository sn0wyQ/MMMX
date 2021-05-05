#ifndef SINGLE_ROOM_INFO_H_
#define SINGLE_ROOM_INFO_H_

#include <memory>
#include <utility>

#include <QDebug>
#include <QLabel>
#include <QResizeEvent>
#include <QWidget>

#include "Client/client_game_model.h"
#include "Server/Room/room_info.h"

class SingleRoomInfo : public QWidget {
 public:
  SingleRoomInfo(QWidget* parent,
                 std::shared_ptr<ClientGameModel> model,
                 const RoomInfo& room_info);

  void UpdateInfo(const RoomInfo& room_info);

  void resizeEvent(QResizeEvent* event) override;

 private:
  std::shared_ptr<ClientGameModel> model_;

  QLabel* label_;
};

#endif  // SINGLE_ROOM_INFO_H_
