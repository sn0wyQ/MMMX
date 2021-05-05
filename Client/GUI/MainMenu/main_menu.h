#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <utility>

#include <QDebug>
#include <QPushButton>
#include <QResizeEvent>
#include <QWidget>

#include "Client/GUI/MainMenu/RoomsInfoList/rooms_info_list.h"
#include "Client/GUI/abstract_client_view.h"
#include "Client/client_controller.h"

class MainMenu : public QWidget {
  Q_OBJECT

 public:
  MainMenu(AbstractClientView* parent,
           std::shared_ptr<ClientController> controller);

  void UpdateRoomsInfoList();

  void resizeEvent(QResizeEvent* event) override;

 public Q_SLOTS:
  void OnStartGameButtonClicked();

 private:
  AbstractClientView* parent_;
  std::shared_ptr<ClientController> controller_;

  RoomsInfoList* rooms_info_list_;
  QPushButton* start_game_;
};

#endif  // MAIN_MENU_H
