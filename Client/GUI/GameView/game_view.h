#ifndef GAME_VIEW_H_
#define GAME_VIEW_H_

#include <memory>
#include <utility>

#include <QLabel>
#include <QResizeEvent>
#include <QWidget>

#include "Client/GUI/GameView/PlayerBar/player_bar.h"
#include "Client/GUI/GameView/ViewPort/viewport.h"
#include "Client/GUI/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Client/client_game_model.h"

class GameView : public QWidget {
 public:
  GameView(AbstractClientView* parent,
           std::shared_ptr<ClientController> controller);

  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  AbstractClientView* parent_;
  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<ClientGameModel> model_;

  PlayerBar* player_bar_;
  QLabel* info_label_;
  ViewPort* view_port_;
};

#endif  // GAME_VIEW_H
