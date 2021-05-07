#ifndef CLIENT_CLIENT_VIEW_H_
#define CLIENT_CLIENT_VIEW_H_

#include <memory>
#include <utility>
#include <vector>

#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>

#include "Client/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Event/event.h"
#include "GUI/GameView/game_view.h"
#include "GUI/GameView/player_bar.h"
#include "GUI/GameView/reloading_field.h"
#include "GUI/GameView/stats_table.h"

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(std::shared_ptr<ClientController> controller);
  ~ClientView() override = default;

  void Update() override;
  std::shared_ptr<Converter> GetConverter() override;

 private:
  void focusOutEvent(QFocusEvent* focus_event) override;
  void keyPressEvent(QKeyEvent* key_event) override;
  void keyReleaseEvent(QKeyEvent* key_event) override;
  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void mousePressEvent(QMouseEvent* mouse_event) override;
  void mouseReleaseEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<ClientGameModel> model_;
  GameView* game_view_;
  PlayerBar* player_bar_;
  ReloadingField* reloading_field_;
  StatsTable* stats_table_;
  QLabel* info_label_;
  int height_of_bar_{};
};

#endif  // CLIENT_CLIENT_VIEW_H_
