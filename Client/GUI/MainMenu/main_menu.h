#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <QPushButton>
#include <QWidget>

#include "Client/GUI/abstract_client_view.h"

class MainMenu : public QWidget {
 public:
  MainMenu(AbstractClientView* parent);

 public Q_SLOTS:
  void OnStartGameButtonClicked();

 private:
  AbstractClientView* parent_;

  QPushButton* start_game_;
};

#endif  // MAIN_MENU_H
