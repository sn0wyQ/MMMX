#ifndef GUI_MAINMENU_MAIN_MENU_H_
#define GUI_MAINMENU_MAIN_MENU_H_

#include <memory>
#include <utility>

#include <QDebug>
#include <QPushButton>
#include <QResizeEvent>
#include <QWidget>

#include "GUI/MainMenu/RoomsInfoList/rooms_info_list.h"
#include "GUI/abstract_client_view.h"
#include "Client/client_controller.h"

class MainMenu : public QWidget {
  Q_OBJECT

 public:
  MainMenu(AbstractClientView* parent, ClientController* controller);

  void UpdateRoomsInfoList();

  void resizeEvent(QResizeEvent* event) override;

  public Q_SLOTS:
  void OnStartGameButtonClicked();
  void OnOpenSettingsButtonClicked();
  void OnQuitAppButtonClicked();

 private:
  AbstractClientView* parent_;
  ClientController* controller_;

  RoomsInfoList* rooms_info_list_;
  QPushButton* start_game_;
  QPushButton* open_settings_;
  QPushButton* quit_app_;
};

#endif  // GUI_MAINMENU_MAIN_MENU_H_
