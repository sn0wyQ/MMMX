#ifndef CLIENT_CLIENT_VIEW_H_
#define CLIENT_CLIENT_VIEW_H_

#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QPainter>

#include "Client/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Converter/converter.h"
#include "Event/event.h"
#include "Painter/painter.h"

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(std::shared_ptr<ClientController> controller);
  ~ClientView() override = default;

  void Update() override;
  void ResetDirection();

 private:
  void keyPressEvent(QKeyEvent* key_event) override;
  void keyReleaseEvent(QKeyEvent* key_event) override;
  void mouseMoveEvent(QMouseEvent* mouse_event) override;
  void paintEvent(QPaintEvent* paint_event) override;
  void resizeEvent(QResizeEvent* resize_event) override;

  std::shared_ptr<ClientController> controller_;
  std::shared_ptr<Converter> converter_;
};

#endif  // CLIENT_CLIENT_VIEW_H_
