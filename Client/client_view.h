#ifndef CLIENT_CLIENT_VIEW_H_
#define CLIENT_CLIENT_VIEW_H_

#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QRandomGenerator>
#include <QPainter>

#include "Client/abstract_client_view.h"
#include "Client/client_controller.h"
#include "Event/event.h"

enum class Direction {
  kUp,
  kRight,
  kDown,
  kLeft,
  SIZE
};

class ClientView : public AbstractClientView {
  Q_OBJECT

 public:
  explicit ClientView(std::shared_ptr<ClientController> controller);
  ~ClientView() override = default;

  void Update() override;
  void ResetDirection();

  public Q_SLOTS:
  void ApplyDirection();

 private:
  void paintEvent(QPaintEvent* paint_event) override;
  void keyPressEvent(QKeyEvent* key_event) override;
  void keyReleaseEvent(QKeyEvent* key_event) override;

  std::shared_ptr<ClientController> controller_;
  std::unordered_map<uint32_t, Direction> key_to_direction_{
      {87, Direction::kUp},
      {68, Direction::kRight},
      {83, Direction::kDown},
      {65, Direction::kLeft}
  };
  std::unordered_map<Direction, bool> is_direction_by_keys_{false};

  std::unordered_map<Direction, bool> is_direction_applied_{false};

  QTimer* timer_for_keys_;
};

#endif  // CLIENT_CLIENT_VIEW_H_
