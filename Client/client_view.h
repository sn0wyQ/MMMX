#ifndef CLIENT_CLIENT_VIEW_H_
#define CLIENT_CLIENT_VIEW_H_

#include <memory>
#include <utility>
#include <vector>

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QPainter>

#include "Client/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Event/event.h"
#include "GUI/GameView/game_view.h"

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(std::shared_ptr<ClientController> controller);
  ~ClientView() override = default;

  void Update() override;
  std::shared_ptr<Converter> GetConverter() override;

 private:
  void keyPressEvent(QKeyEvent* key_event) override;
  void keyReleaseEvent(QKeyEvent* key_event) override;
  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

  std::shared_ptr<ClientController> controller_;
  GameDataModel* model_;

  GameView* game_view;
  QLabel* info_label_;
};

#endif  // CLIENT_CLIENT_VIEW_H_
